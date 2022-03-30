#ifndef DATA_STRUCTURES_AND_ALGORITHM_CONNSTATE_H
#define DATA_STRUCTURES_AND_ALGORITHM_CONNSTATE_H

#include <cstdlib>

#include <memory>
#include <mutex>
#include <string>

#include <asio.hpp>

class TCPConn;

class ConnState {
public:
    using tcp_p = std::shared_ptr<TCPConn>;
    virtual ssize_t parseInRead(tcp_p conn, const char *msg, ssize_t sz) = 0;
    virtual ssize_t parseInWrite(tcp_p conn, const char *msg, ssize_t sz) = 0;
    virtual ssize_t parseOutRead(tcp_p conn, const char *msg, ssize_t sz) { return sz; };
    virtual ssize_t parseOutWrite(tcp_p conn, const char *msg, ssize_t sz) { return sz; };
    virtual void setState(tcp_p conn, ConnState *state);
};

class Auth : public ConnState {
public:
    static ConnState *instance();
    ssize_t parseInRead(tcp_p conn, const char *msg, ssize_t sz) override;
    ssize_t parseInWrite(tcp_p conn, const char *msg, ssize_t sz) override;

private:
    Auth() = default;
    static Auth *_self;
    static std::once_flag _of;
};

class URLReq : public ConnState {
public:
    static ConnState *instance();
    ssize_t parseInRead(tcp_p conn, const char *msg, ssize_t sz) override;
    ssize_t parseInWrite(tcp_p conn, const char *msg, ssize_t sz) override;

    void doConnect(tcp_p conn, std::string address, uint16_t port, asio::ip::tcp::endpoint ep);

private:
    URLReq() = default;
    static URLReq *_self;
    static std::once_flag _of;
};

class Established : public ConnState {
public:
    static ConnState *instance();
    ssize_t parseInRead(tcp_p conn, const char *msg, ssize_t sz) override;
    ssize_t parseInWrite(tcp_p conn, const char *msg, ssize_t sz) override;
    ssize_t parseOutRead(tcp_p conn, const char *msg, ssize_t sz) override;
    ssize_t parseOutWrite(tcp_p conn, const char *msg, ssize_t sz) override;

private:
    Established() = default;
    static Established *_self;
    static std::once_flag _of;
};

#endif //DATA_STRUCTURES_AND_ALGORITHM_CONNSTATE_H
