#include "ConnState.h"

#include <iostream>
#include <mutex>

#include "TCPConn.h"

using namespace std;


Auth *Auth::_self;
std::once_flag Auth::_of;
URLReq *URLReq::_self;
std::once_flag URLReq::_of;
Established *Established::_self;
std::once_flag Established::_of;

void ConnState::setState(ConnState::tcp_p conn, ConnState *state) {
    conn->setState(state);
}

ConnState *Auth::instance() {
    std::call_once(_of, [] { _self = new Auth; });
    return _self;
}

ssize_t Auth::parseInRead(tcp_p conn, const char *msg, ssize_t sz) {
    if (sz != 3 || msg[0] != 0x05 || msg[1] != 0x01 || msg[2] != 0x00) {
        cout << conn->_id << " unsupported socks5" << endl;
        return -1;
    }
    memcpy(conn->_in_write_buf, "\x05\x00", 2);
    conn->_in_write_len = 2;
    conn->in_write();
    return sz;
}

ssize_t Auth::parseInWrite(ConnState::tcp_p conn, const char *msg, ssize_t sz) {
    if (sz != 2 || msg[0] != 0x05 || msg[1] != 0x00) {
        cout << conn->_id << " return package corrupt" << endl;
        return -1;
    }
    setState(conn, URLReq::instance());
    conn->in_read();
    return sz;
}

ConnState *URLReq::instance() {
    std::call_once(_of, [] { _self = new URLReq; });
    return _self;
}

ssize_t URLReq::parseInRead(tcp_p conn, const char *msg, ssize_t sz) {
    if (sz < 5)
        return -1;
    int expected_len;
    uint8_t type = msg[3];
    if (type == 0x01)
        expected_len = 10;
    else if (type == 0x03)
        expected_len = uint8_t(msg[4]) + 7;
    else if (type == 0x04)
        expected_len = 22;
    else
        return -1;

    if (expected_len != sz)
        return -1;

    string address;
    uint16_t port = ((uint16_t) *(uint8_t *) (msg + sz - 2) << 8) + (uint16_t) *(uint8_t *) (msg + sz - 1);
    if (type == 0x01) {     // parse IP
        address.append(to_string((uint8_t) msg[5]))
                .append(1, '.')
                .append(to_string((uint8_t) msg[6]))
                .append(1, '.')
                .append(to_string((uint8_t) msg[7]))
                .append(1, '.')
                .append(to_string((uint8_t) msg[8]));
        auto ep = ip::tcp::endpoint(ip::address::from_string(address), port);
        doConnect(conn, std::move(address), port, std::move(ep));
    } else if (type == 0x03) {  // parse DOMAIN
        address.append(msg + 5, (uint8_t) msg[4]);
        conn->_resolver.async_resolve(
                address, to_string(port),
                [this, conn, address, port](const error_code err, ip::tcp::resolver::iterator it) {
                    if (err) {
                        cout << conn->_id << " resolve error: " << address << ":" << port << ": " << err.message()
                             << endl;
                        return;
                    }
                    auto ep = *it;
                    doConnect(conn, address, port, ep);
                });
    } else {
        cout << conn->_id << " unsupported address type" << endl;
        return -1;
    }

    return sz;
}

void URLReq::doConnect(tcp_p conn, string address, uint16_t port, ip::tcp::endpoint ep) {
    cout << conn->_id << " " << address << ":" << port << " connecting" << endl;
    conn->_out_sock->async_connect(
            ep,
            [conn, address, port](const error_code err) {
                if (err) {
                    cout << conn->_id << " " << address << ":" << port << " connect error" << endl;
                    return;
                }
                cout << conn->_id << " " << address << ":" << port << " connected" << endl;
                conn->_remote_addr = address;
                conn->_remote_port = port;
                memcpy(conn->_in_write_buf, "\x05\x00\x00\x01\x00\x00\x00\x00\x00\x00", 10);
                conn->_in_write_buf[3] = 1;
                conn->_in_write_len = 10;
                conn->in_write();
            });
}

ssize_t URLReq::parseInWrite(ConnState::tcp_p conn, const char *msg, ssize_t sz) {
    if (memcmp(msg, "\x05\x00\x00\x01\x00\x00\x00\x00\x00\x00", 10) != 0)
        return -1;
    setState(conn, Established::instance());
    conn->out_read();
    conn->in_read();
    return sz;
}

ConnState *Established::instance() {
    std::call_once(_of, [] { _self = new Established; });
    return _self;
}

ssize_t Established::parseInRead(tcp_p conn, const char *msg, ssize_t sz) {
    memcpy(conn->_out_write_buf, msg, sz);
    conn->_out_write_len = sz;
    conn->out_write();
    return sz;
}

ssize_t Established::parseInWrite(tcp_p conn, const char *msg, ssize_t sz) {
    conn->out_read();
    return sz;
}

ssize_t Established::parseOutRead(tcp_p conn, const char *msg, ssize_t sz) {
    memcpy(conn->_in_write_buf, msg, sz);
    conn->_in_write_len = sz;
    conn->in_write();
    return sz;
}

ssize_t Established::parseOutWrite(tcp_p conn, const char *msg, ssize_t sz) {
    conn->in_read();
    return sz;
}
