#include "direct.h"

#include <iostream>

#include "TCPConn.h"

using namespace std;

DirectDialer *DirectDialer::_self;
std::once_flag DirectDialer::_of;

ssize_t DirectDialer::parseInRead(conn_p conn, const uint8_t *msg, ssize_t sz) {
    // memcpy(conn->_out_write_buf, msg, sz);
    // memcpy(conn->_in_to_out_buf, msg, sz);
    conn->outWrite();
    return sz;
}

ssize_t DirectDialer::parseInWrite(conn_p conn, const uint8_t *msg, ssize_t sz) {
    conn->outRead();
    return sz;
}

ssize_t DirectDialer::parseOutRead(conn_p conn, const uint8_t *msg, ssize_t sz) {
    // memcpy(conn->_in_write_buf, msg, sz);
    // memcpy(conn->_out_to_in_buf, msg, sz);
    conn->inWrite();
    return sz;
}

ssize_t DirectDialer::parseOutWrite(conn_p conn, const uint8_t *msg, ssize_t sz) {
    conn->inRead();
    return sz;
}

void DirectDialer::doDial(conn_p conn) {
    cout << conn->_id << " " << conn->_remote_addr << ":" << conn->_remote_port << " connecting" << endl;
    if (conn->_conn_type == ConnType::UDP)
        return;
    if (conn->_addr_type == AddrType::Domain) {
        conn->_resolver.async_resolve(
                conn->_remote_addr, to_string(conn->_remote_port),
                [this, conn](const error_code err, ip::tcp::resolver::iterator it) {
                    if (err)
                        return;
                    if (conn->_conn_type == ConnType::TCP)
                        doTCPDial(conn, *it);
                }
        );
    } else {
        if (conn->_conn_type == ConnType::TCP)
            doTCPDial(conn, ip::tcp::endpoint(ip::address::from_string(conn->_remote_addr), conn->_remote_port));
    }
}

void DirectDialer::doTCPDial(conn_p conn, ip::tcp::endpoint ep) {
    conn->_out_sock->async_connect(
            ep, [conn](const error_code err) {
                if (err) {
                    cout << conn->_id << " " << conn->_remote_addr << ":"
                         << conn->_remote_port << " connect error" << endl;
                    return;
                }
                cout << conn->_id << " " << conn->_remote_addr << ":" << conn->_remote_port << " connected" << endl;
                conn->outRead();
                conn->inRead();
            }
    );
}

void DirectDialer::doUDPDial(conn_p conn, ip::udp::endpoint ep) {

}

DirectDialer *DirectDialer::instance() {
    std::call_once(_of, [] { _self = new DirectDialer; });
    return _self;
}
