#ifndef NETCAT_TCP_H
#define NETCAT_TCP_H

#include "connection.h"

class ConnHolder;

/**
 * @brief TCPIn is TCP implementation of in socket.
 */
class TCPIn : public InConn {
public:
    TCPIn(tcp_sock_p p, ConnHolder *holder, AcceptStrategy *strategy);

    void inRead(holder_p holder) override;
    void inWrite(holder_p holder) override;
    void closeMe(CloseType type) override;

private:
    tcp_sock_p _in_sock;
};

/**
 * @brief TCPOut is TCP implementation of out socket.
 */
class TCPOut : public OutConn {
public:
    TCPOut(ConnHolder *holder, DialStrategy *strategy);

    void outRead(holder_p holder) override;
    void outWrite(holder_p holder) override;
    void closeMe(CloseType type) override;

    void dial(holder_p holder/* TODO: route info */) override;

private:
    tcp_sock_p _out_sock;
    ip::tcp::resolver _resolver;

    void dialHelper(ip::tcp::endpoint ep, holder_p holder);
};

#endif