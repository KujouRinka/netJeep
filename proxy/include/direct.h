#ifndef NETCAT_DIRECT_H
#define NETCAT_DIRECT_H

#include "proxy_interface.h"

#include <string>
#include <mutex>

#include <asio.hpp>

class DirectDialer : public Dialer {
public:
    static DirectDialer *instance();
    ssize_t parseInRead(conn_p conn, conn_p holder) override;
    ssize_t parseInWrite(conn_p conn, conn_p holder) override;
    ssize_t parseOutRead(conn_p conn, conn_p holder) override;
    ssize_t parseOutWrite(conn_p conn, conn_p holder) override;

private:
    static DirectDialer*_self;
    static std::once_flag _of;
};

#endif