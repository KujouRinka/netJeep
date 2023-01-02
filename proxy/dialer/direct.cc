#include "proxy/direct.h"

#include "connection/conn_holder.h"

using namespace proxy::Direct;

proxy::DialStrategy *Dialer::_self;

ssize_t Dialer::onOutRead(ConnHolder *holder, OutConn *out) {
  holder->toInWrite();
  return 0;
}

ssize_t Dialer::onOutWrite(ConnHolder *holder, OutConn *out) {
  holder->toInRead();
  return 0;
}

void Dialer::init() {
  if (_self == nullptr)
    _self = new Dialer;
}

proxy::DialStrategy *Dialer::startStat() {
  return _self;
}

dialCoreBuilder proxy::Direct::dialBuilderFromConfig(Config::Dialer &d) {
  Dialer::init();
  return [](ConnHolder *holder) -> dialCore {
    return {Dialer::startStat(), nullptr};
  };
}
