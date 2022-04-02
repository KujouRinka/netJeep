#include "socks5.h"

#include <iostream>
#include <mutex>

#include "TCPConn.h"
#include "direct.h"
#include "types.h"

using namespace std;

// TODO: there exists a high performance solution
//  to instance these.
Auth *Auth::_self;
std::once_flag Auth::_of;
URLReq *URLReq::_self;
std::once_flag URLReq::_of;

SocksListener *SocksListener::getStart() {
    return Auth::instance();
}

SocksListener *Auth::instance() {
    std::call_once(_of, [] { _self = new Auth; });
    return _self;
}

ssize_t Auth::parseInRead(conn_p conn, const uint8_t *msg, ssize_t sz) {
    if (sz < 3 || msg[0] != 0x05 || !msg[1] || msg[2] != 0x00) {
        cout << conn->_id << " unsupported socks5" << endl;
        return -1;
    }
    //memcpy(conn->_in_write_buf, "\x05\x00", 2);
    memcpy(conn->_out_to_in_buf, "\x05\x00", 2);
    conn->_in_write_len = 2;
    conn->inWrite();
    return sz;
}

ssize_t Auth::parseInWrite(conn_p conn, const uint8_t *msg, ssize_t sz) {
    if (sz != 2 || msg[0] != 0x05 || msg[1] != 0x00) {
        cout << conn->_id << " return package corrupt" << endl;
        return -1;
    }
    setHandler(conn, URLReq::instance());
    conn->inRead();
    return sz;
}

SocksListener *URLReq::instance() {
    std::call_once(_of, [] { _self = new URLReq; });
    return _self;
}

ssize_t URLReq::parseInRead(conn_p conn, const uint8_t *msg, ssize_t sz) {
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

    if (msg[1] == 0x01)
        conn->_conn_type = ConnType::TCP;
    else if (msg[1] == 0x03)
        conn->_conn_type = ConnType::UDP;


    string address;
    uint16_t port = ((uint16_t) *(msg + sz - 2) << 8) + *(msg + sz - 1);
    if (type == 0x01) {     // parse IPv4
        conn->_addr_type = AddrType::IPv4;
        address.append(to_string(msg[5]))
                .append(1, '.')
                .append(to_string(msg[6]))
                .append(1, '.')
                .append(to_string(msg[7]))
                .append(1, '.')
                .append(to_string(msg[8]));
    } else if (type == 0x03) {  // parse DOMAIN
        conn->_addr_type = AddrType::Domain;
        address.append(reinterpret_cast<const char *>(msg + 5), msg[4]);
    } else {
        cout << conn->_id << " unsupported address type" << endl;
        return -1;
    }
    conn->_remote_addr = std::move(address);
    conn->_remote_port = port;

    memcpy(conn->_out_to_in_buf, "\x05\x00\x00\x01\x00\x00\x00\x00\x00\x00", 10);
    conn->_in_write_len = 10;
    conn->inWrite();

    return sz;
}

ssize_t URLReq::parseInWrite(conn_p conn, const uint8_t *msg, ssize_t sz) {
    if (memcmp(msg, "\x05\x00\x00\x01\x00\x00\x00\x00\x00\x00", 10) != 0)
        return -1;
    conn->transferToDialer(DirectDialer::instance());
    return sz;
}
