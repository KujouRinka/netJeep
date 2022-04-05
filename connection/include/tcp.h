#ifndef NETCAT_TCP_H
#define NETCAT_TCP_H

#include "connection.h"

class TCPIn : public InConn {
public:
    TCPIn(io_context &ctx, tcp_sock_p p, ConnHandler *h, long long id)
            : InConn(ctx, h, id), _in_sock(p) {}
    ~TCPIn() override;

    // hold OutConn
    void inRead(conn_p holder) override;
    void inWrite(conn_p holder) override;
    void closeMe(CloseType type) override;

private:
    tcp_sock_p _in_sock;
};


class TCPOut : public OutConn {
public:
    TCPOut(conn_p p) : OutConn(p), _out_sock(new tcp_sock(_ctx)), _resolver(p->getCtx()) {};
    ~TCPOut() override;

    // hold InConn
    void outRead(conn_p holder) override;
    void outWrite(conn_p holer) override;
    void closeMe(CloseType type) override;

    void dial(conn_p in_holder) override;

private:
    tcp_sock_p _out_sock;
    ip::tcp::resolver _resolver;

    void dialHelper(ip::tcp::endpoint ep, conn_p in_holder);
};

#endif