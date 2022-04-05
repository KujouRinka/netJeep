#ifndef NETCAT_SOCKS5_H
#define NETCAT_SOCKS5_H

#include "proxy_interface.h"

#include <cstdint>

#include <mutex>
#include <string>

#include "types.h"


/**
 * @brief SocksAuthenticator implements how to accept socks request
 *
 * @todo implement fully socks support, not just socks5
 */
class SocksAuthenticator : public Authenticator {
public:
    static SocksAuthenticator *startStat();
};

class Auth : public SocksAuthenticator {
public:
    static SocksAuthenticator *instance();
    ssize_t parseInRead(conn_p conn, conn_p holder) override;
    ssize_t parseInWrite(conn_p conn, conn_p holder) override;

private:
    Auth() = default;
    static Auth *_self;
    static std::once_flag _of;
};

class URLReq : public SocksAuthenticator {
public:
    static SocksAuthenticator *instance();
    ssize_t parseInRead(conn_p conn, conn_p holder) override;
    ssize_t parseInWrite(conn_p conn, conn_p holder) override;

private:
    URLReq() = default;
    static URLReq *_self;
    static std::once_flag _of;
};

#endif
