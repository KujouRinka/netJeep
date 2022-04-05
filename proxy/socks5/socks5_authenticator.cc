#include "socks5.h"

#include <iostream>
#include <mutex>

#include "types.h"
#include "connection.h"

using namespace std;

// TODO: there exists a high performance solution
//  to instance these.
Auth *Auth::_self;
std::once_flag Auth::_of;
URLReq *URLReq::_self;
std::once_flag URLReq::_of;

SocksAuthenticator *SocksAuthenticator::startStat() {
    return Auth::instance();
}

SocksAuthenticator *Auth::instance() {
    std::call_once(_of, [] { _self = new Auth; });
    return _self;
}

ssize_t Auth::parseInRead(conn_p conn, conn_p holder) {
    uint8_t *msg = conn->inToOutBuf()->get();
    if (conn->inToOutBuf()->size() < 3 || msg[0] != 0x05 || !msg[1] || msg[2] != 0x00) {
        cout << conn->id() << " unsupported socks5" << endl;
        return -1;
    }
    conn->outToInBuf()->rewrite("\x05\x00", 2);
    conn->inWrite(holder);
    return 0;
}

ssize_t Auth::parseInWrite(conn_p conn, conn_p holder) {
    uint8_t *msg = conn->outToInBuf()->get();
    if (conn->outToInBuf()->size() != 2 || msg[0] != 0x05 || msg[1] != 0x00 ) {
        cout << conn->id() << " return package corrupt" << endl;
        return -1;
    }
    conn->connHandler() = URLReq::instance();
    conn->inRead(holder);
    return 0;
}

SocksAuthenticator *URLReq::instance() {
    std::call_once(_of, [] { _self = new URLReq; });
    return _self;
}

ssize_t URLReq::parseInRead(conn_p conn, conn_p holder) {
    uint8_t *msg = conn->inToOutBuf()->get();
    ssize_t buf_size = conn->inToOutBuf()->size();
    if (buf_size < 5)
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

    if (expected_len != buf_size)
        return -1;

    if (msg[1] == 0x01)
        conn->remote().conn_type() = ConnType::TCP;
    else if (msg[1] == 0x03)
        conn->remote().conn_type() = ConnType::UDP;


    string address;
    uint16_t port = ((uint16_t) *(msg + buf_size - 2) << 8) + *(msg + buf_size - 1);
    if (type == 0x01) {     // parse IPv4
        conn->remote().addr_type() = AddrType::IPv4;
        address.append(to_string(msg[5]))
                .append(1, '.')
                .append(to_string(msg[6]))
                .append(1, '.')
                .append(to_string(msg[7]))
                .append(1, '.')
                .append(to_string(msg[8]));
    } else if (type == 0x03) {  // parse DOMAIN
        conn->remote().addr_type() = AddrType::Domain;
        address.append(reinterpret_cast<const char *>(msg + 5), msg[4]);
    } else {
        cout << conn->id() << " unsupported address type" << endl;
        return -1;
    }
    conn->remote().addr() = std::move(address);
    conn->remote().port() = port;

    conn->outToInBuf()->rewrite("\x05\x00\x00\x01\x00\x00\x00\x00\x00\x00", 10);
    conn->inWrite(holder);

    return 0;
}

ssize_t URLReq::parseInWrite(conn_p conn, conn_p holder) {
    if (memcmp(conn->outToInBuf()->get(), "\x05\x00\x00\x01\x00\x00\x00\x00\x00\x00", 10) != 0)
        return -1;

    conn->dial(conn);
    return 0;
}
