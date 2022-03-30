#ifndef DATA_STRUCTURES_AND_ALGORITHM_TCPCONN_H
#define DATA_STRUCTURES_AND_ALGORITHM_TCPCONN_H

#include <asio.hpp>
#include <memory>

using namespace std;
using namespace asio;

class ConnState;

class TCPConn : public enable_shared_from_this<TCPConn> {
    using tcp_sock = ip::tcp::socket;
    using tcp_sock_p = shared_ptr<tcp_sock>;
    friend class ConnState;
    friend class Auth;
    friend class URLReq;
    friend class Established;
    static constexpr size_t BUF_SIZE = 2048;

public:
    TCPConn(io_context &ctx, tcp_sock_p sock_p, long long conn_id);
    ~TCPConn();

    void in_read();
    void out_write();
    void out_read();
    void in_write();

    void close();

private:
    long long _id;

    io_context &_ctx;
    tcp_sock_p _in_sock;
    tcp_sock_p _out_sock;

    string _remote_addr;
    uint16_t _remote_port;

    char _in_read_buf[BUF_SIZE];
    char _in_write_buf[BUF_SIZE];
    char _out_read_buf[BUF_SIZE];
    char _out_write_buf[BUF_SIZE];

    ssize_t _in_write_len;
    ssize_t _out_write_len;

    ip::tcp::resolver _resolver;

    ConnState *_state;

    void setState(ConnState *state);
};

#endif //DATA_STRUCTURES_AND_ALGORITHM_TCPCONN_H
