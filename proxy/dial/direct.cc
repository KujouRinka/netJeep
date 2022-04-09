#include "direct.h"

#include "conn_holder.h"

DialStrategy *DialDirect::_self;
std::once_flag DialDirect::_of;

ssize_t DialDirect::onOutRead(ConnHolder *holder, OutConn *out) {
    holder->inWrite();
    return 0;
}

ssize_t DialDirect::onOutWrite(ConnHolder *holder, OutConn *out) {
    holder->inRead();
    return 0;
}

DialStrategy *DialDirect::instance() {
    std::call_once(_of, [] { _self = new DialDirect; });
    return _self;
}

DialStrategy *DialDirect::startStat() {
    return DialDirect::instance();
}
