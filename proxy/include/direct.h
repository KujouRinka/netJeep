#ifndef NETCAT_DIRECT_H
#define NETCAT_DIRECT_H

#include <string>
#include <mutex>

#include <asio.hpp>

#include "proxy_interface.h"

class DirectDialer : public Dialer {
public:
    static DirectDialer *instance();
    ssize_t parseInRead(conn_p conn, const uint8_t *msg, ssize_t sz) override;
    ssize_t parseInWrite(conn_p conn, const uint8_t *msg, ssize_t sz) override;
    ssize_t parseOutRead(conn_p conn, const uint8_t *msg, ssize_t sz) override;
    ssize_t parseOutWrite(conn_p conn, const uint8_t *msg, ssize_t sz) override;

    void doDial(conn_p conn) override;

private:
    static DirectDialer*_self;
    static std::once_flag _of;

    void doTCPDial(conn_p conn, asio::ip::tcp::endpoint ep);
    void doUDPDial(conn_p conn, asio::ip::udp::endpoint ep);
};

#endif