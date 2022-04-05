#include "direct.h"
#include "connection.h"

#include <iostream>

#include "buffer.h"

using namespace std;

DirectDialer *DirectDialer::_self;
std::once_flag DirectDialer::_of;

ssize_t DirectDialer::parseInRead(conn_p conn, conn_p holder) {
    conn->outWrite(holder);
    return 0;
}

ssize_t DirectDialer::parseInWrite(conn_p conn, conn_p holder) {
    conn->outRead(holder);
    return 0;
}

ssize_t DirectDialer::parseOutRead(conn_p conn, conn_p holder) {
    conn->inWrite(holder);
    return 0;
}

ssize_t DirectDialer::parseOutWrite(conn_p conn, conn_p holder) {
    conn->inRead(holder);
    return 0;
}

DirectDialer *DirectDialer::instance() {
    std::call_once(_of, [] { _self = new DirectDialer; });
    return _self;
}
