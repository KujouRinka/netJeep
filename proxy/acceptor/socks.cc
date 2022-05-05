#include "proxy/socks.h"

#include <memory>

#include "connection/conn_holder.h"
#include "connection/connection.h"
#include "connection/protocol.h"

#include "config/config.h"

using namespace proxy::Socks;

proxy::AcceptStrategy *Negotiation::_self;
proxy::AcceptStrategy *ReqParse::_self;
proxy::AcceptStrategy *Established::_self;

void Acceptor::init() {
    Negotiation::init();
    ReqParse::init();
    Established::init();
}

proxy::AcceptStrategy *Acceptor::startStat() {
    return Negotiation::instance();
}

ssize_t Negotiation::onInRead(ConnHolder *holder, InConn *in) {
    uint8_t buf[257];
    holder->IOBufStream().read((char *) buf, 2);
    uint8_t n_method = buf[1];
    if (buf[1] != holder->IOBuf().size()) {
        cout << holder->id() << ": bad socks5 handshake: "
             << (int) buf[1] << " != " << (int) holder->IOBuf().size() << endl;
    }
    holder->IOBufStream().read((char *) (buf + 2), n_method);
    // if 0x00 in buffer, OK
    if (n_method <= 0 || buf[0] != 0x05 || buf[2] != 0x00) {
        cout << holder->id() << ": unsupported socks5" << endl;
        return -1;
    }
    holder->OIBufStream().write("\x05\x00", 2);
    holder->inWrite();
    return 0;
}

ssize_t Negotiation::onInWrite(ConnHolder *holder, InConn *in) {
    if (holder->OIBuf().size() != 0) {
        holder->inWrite();
    } else {
        in->strategy() = ReqParse::instance();
        holder->inRead();
    }
    return 0;
}

void Negotiation::init() {
    if (_self == nullptr)
        _self = new Negotiation();
}

proxy::AcceptStrategy *Negotiation::instance() {
    return _self;
}

ssize_t ReqParse::onInRead(ConnHolder *holder, InConn *in) {
    uint16_t buf_size = holder->IOBuf().size();
    if (buf_size < 5) {
        cout << holder->id() << ": invalid socks5 request" << endl;
        return -1;
    }
    uint8_t buffer[262];
    holder->IOBufStream().read((char *) buffer, 5);
    uint16_t expected_len;
    uint8_t type = buffer[3];
    if (type == 0x01) {     // IPv4
        expected_len = 10;
    } else if (type == 0x03) {  // Domain
        expected_len = buffer[4] + 7;
    } else if (type == 0x04) {  // IPv6
        expected_len = 22;
    } else {
        cout << holder->id() << ": unsupported address type" << endl;
        return -1;
    }

    if (expected_len != buf_size)
        return -1;
    holder->IOBufStream().read((char *) (buffer + 5), expected_len - 5);

    if (buffer[1] == 0x01)
        holder->remote().conn_type() = ConnType::TCP;
    else if (buffer[1] == 0x03)
        holder->remote().conn_type() = ConnType::UDP;


    string address;
    uint16_t port = ((uint16_t) *(buffer + buf_size - 2) << 8) + *(buffer + buf_size - 1);
    if (type == 0x01) {     // parse IPv4
        holder->remote().addr_type() = AddrType::IPv4;
        address.append(to_string(buffer[4]))
            .append(1, '.')
            .append(to_string(buffer[5]))
            .append(1, '.')
            .append(to_string(buffer[6]))
            .append(1, '.')
            .append(to_string(buffer[7]));
    } else if (type == 0x03) {  // parse DOMAIN
        holder->remote().addr_type() = AddrType::Domain;
        address.append(reinterpret_cast<const char *>(buffer + 5), buffer[4]);
    } else if (type == 0x04) {  // parse IPv6
        holder->remote().addr_type() = AddrType::IPv6;
        // TODO: parse IPv6, temporary unsupported
        cout << holder->id() << ": unsupported address type" << endl;
        return -1;
    }

    holder->remote().addr() = std::move(address);
    holder->remote().port() = port;

    holder->OIBufStream().write("\x05\x00\x00\x01\x00\x00\x00\x00\x00\x00", 10);
    holder->inWrite();
    return 0;

}

ssize_t ReqParse::onInWrite(ConnHolder *holder, InConn *in) {
    if (holder->OIBuf().size() != 0) {
        holder->inWrite();
    } else {
        in->strategy() = Established::instance();
        holder->dial();
    }
    return 0;
}

void ReqParse::init() {
    if (_self == nullptr)
        _self = new ReqParse();
}

proxy::AcceptStrategy *ReqParse::instance() {
    return _self;
}

ssize_t Established::onInRead(ConnHolder *holder, InConn *in) {
    holder->toOutWrite();
    return 0;
}

ssize_t Established::onInWrite(ConnHolder *holder, InConn *in) {
    holder->toOutRead();
    return 0;
}

void Established::init() {
    if (_self == nullptr)
        _self = new Established();
}

proxy::AcceptStrategy *Established::instance() {
    return _self;
}

void proxy::Socks::acceptTCP(asio::ip::tcp::acceptor *ac, asio::io_context &ctx) {
    tcp_sock_p sp(new tcp_sock(ctx));
    ac->async_accept(*sp, [sp, ac, &ctx](const error_code err) {
        if (!err) {
            std::make_shared<ConnHolder>(
                ctx, make_shared<TCPIn>(sp, nullptr, Acceptor::startStat()),
                0
            )->start();
        }
        acceptTCP(ac, ctx);
    });
}

acceptFunc proxy::Socks::acceptFuncFromConfig(io_context &ctx, Config::Acceptor &a) {
    Acceptor::init();
    if (a.protocol != "socks" || a.detail == nullptr)
        return nullptr;
    auto address = a.detail->address;
    auto port = a.detail->port;
    auto ac = new ip::tcp::acceptor(ctx, ip::tcp::endpoint(ip::address::from_string(address), port));
    return [ac, &ctx]() {
        acceptTCP(ac, ctx);
    };
}
