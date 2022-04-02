#ifndef NETCAT_SOCKS5_H
#define NETCAT_SOCKS5_H

#include <cstdint>

#include <memory>
#include <mutex>
#include <string>

#include <asio.hpp>

#include "proxy_interface.h"
#include "types.h"

class RealConn;

class SocksListener : public Listener {
public:
    static SocksListener *getStart();
};

class Auth : public SocksListener {
public:
    static SocksListener *instance();
    ssize_t parseInRead(conn_p conn, const uint8_t *msg, ssize_t sz) override;
    ssize_t parseInWrite(conn_p conn, const uint8_t *msg, ssize_t sz) override;

private:
    Auth() = default;
    static Auth *_self;
    static std::once_flag _of;
};

class URLReq : public SocksListener {
public:
    static SocksListener *instance();
    ssize_t parseInRead(conn_p conn, const uint8_t *msg, ssize_t sz) override;
    ssize_t parseInWrite(conn_p conn, const uint8_t *msg, ssize_t sz) override;

private:
    URLReq() = default;
    static URLReq *_self;
    static std::once_flag _of;
};

#endif //NETCAT_SOCKS5_H
