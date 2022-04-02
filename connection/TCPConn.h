#ifndef NETCAT_TCPCONN_H
#define NETCAT_TCPCONN_H

#include <asio.hpp>
#include <memory>

#include "connection.h"
#include "types.h"
#include "proxy_interface.h"

using namespace std;
using namespace asio;

class RealConn;
class ConnHandler;

/**
 * @brief RealConn does real data transfer between Listener and Dialer.
 *
 * RealConn does real job of transferring data from _in_sock to _out_sock
 * and vice versa. RealConn is driven by a finite state machine referencing
 * ConnHandler.
 *
 */
class RealConn
        : public enable_shared_from_this<RealConn>,
          public Connection {
    friend ConnHandler;
    friend class Auth;
    friend class URLReq;
    friend class Established;
    static constexpr size_t BUF_SIZE = 2048;

public:
    RealConn(io_context &ctx, tcp_sock_p sock_p, ConnHandler *handler, long long conn_id);
    ~RealConn() override;

    void inRead() override;
    void inWrite() override;
    void outRead() override;
    void outWrite() override;

    // TODO: use Timer to close inactive connection
    void close();

    void transferToDialer(Dialer *dialer);

    // TODO: replace by high performance and flexibility buffer
    uint8_t _out_to_in_buf[BUF_SIZE];
    uint8_t _in_to_out_buf[BUF_SIZE];
    ssize_t _in_write_len;
    ssize_t _out_write_len;

// private:
    long long _id;

    io_context &_ctx;
    tcp_sock_p _in_sock;
    tcp_sock_p _out_sock;

    // out_sock_p _out_sock;
    // out_sock_union *_out_sock;

    string _remote_addr;
    uint16_t _remote_port;

    //uint8_t _in_read_buf[BUF_SIZE];
    //uint8_t _in_write_buf[BUF_SIZE];
    //uint8_t _out_read_buf[BUF_SIZE];
    //uint8_t _out_write_buf[BUF_SIZE];

    ip::tcp::resolver _resolver;

    ConnHandler *_handler;
    ConnType _conn_type;
    AddrType _addr_type;

    void setHandler(ConnHandler *handler);
};

#endif //NETCAT_TCPCONN_H
