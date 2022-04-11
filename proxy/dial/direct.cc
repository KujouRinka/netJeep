#include "proxy/direct.h"

#include "connection/conn_holder.h"

using namespace proxy::Direct;

proxy::DialStrategy *Dialer::_self;
std::once_flag Dialer::_of;

ssize_t Dialer::onOutRead(ConnHolder *holder, OutConn *out) {
    holder->toInWrite();
    return 0;
}

ssize_t Dialer::onOutWrite(ConnHolder *holder, OutConn *out) {
    holder->toInRead();
    return 0;
}

proxy::DialStrategy *Dialer::instance() {
    std::call_once(_of, [] { _self = new Dialer; });
    return _self;
}

proxy::DialStrategy *Dialer::startStat() {
    return Dialer::instance();
}
