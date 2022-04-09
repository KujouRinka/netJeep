#ifndef NETCAT_SOCKS_H
#define NETCAT_SOCKS_H

#include "proxy_interface.h"

#include <cstdint>

#include <mutex>
#include <string>

#include "types.h"

/**
 * @brief AcceptSocks implements socks handshake of passive endpoints.
 *
 * @details https://en.wikipedia.org/wiki/SOCKS
 *
 */
class AcceptSocks : public AcceptStrategy {
public:
    ssize_t onInRead(ConnHolder *holder, InConn *in) override = 0;
    ssize_t onInWrite(ConnHolder *holder, InConn *in) override = 0;

    static AcceptStrategy *startStat();
};

class Auth : public AcceptSocks {
public:
    ssize_t onInRead(ConnHolder *holder, InConn *in) override;
    ssize_t onInWrite(ConnHolder *holder, InConn *in) override;
    static AcceptStrategy *instance();

private:
    Auth() = default;
    static AcceptStrategy *_self;
    static std::once_flag _of;
};

class URLReq : public AcceptSocks {
public:
    ssize_t onInRead(ConnHolder *holder, InConn *in) override;
    ssize_t onInWrite(ConnHolder *holder, InConn *in) override;
    static AcceptStrategy *instance();

private:
    URLReq() = default;
    static AcceptStrategy *_self;
    static std::once_flag _of;
};

class Established : public AcceptSocks {
public:
    ssize_t onInRead(ConnHolder *holder, InConn *in) override;
    ssize_t onInWrite(ConnHolder *holder, InConn *in) override;
    static AcceptStrategy *instance();

private:
    Established() = default;
    static AcceptStrategy *_self;
    static std::once_flag _of;
};

#endif
