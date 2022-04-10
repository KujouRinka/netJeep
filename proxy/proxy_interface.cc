#include "proxy_interface.h"

#include "connection.h"
#include "conn_holder.h"

ssize_t AcceptStrategy::toInRead(ConnHolder *holder, InConn *in) {
    holder->inRead();
    return 0;
}

ssize_t AcceptStrategy::toInWrite(ConnHolder *holder, InConn *in) {
    holder->inWrite();
    return 0;
}

void AcceptStrategy::stop() {}

ssize_t DialStrategy::toOutRead(ConnHolder *holder, OutConn *out) {
    holder->outRead();
    return 0;
}

ssize_t DialStrategy::toOutWrite(ConnHolder *holder, OutConn *out) {
    holder->outWrite();
    return 0;
}

void DialStrategy::stop() {}