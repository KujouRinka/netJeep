#include "conn_holder.h"

#include "tcp.h"

#include "router/router.h"
#include "cipher/cipher.h"

ConnHolder::ConnHolder(asio::io_context &ctx, in_p in, long long id)
    : _ctx(ctx), _in(std::move(in)), _id(id),
      _in_to_out_buf(MAX_BUF_SIZE),
      _out_to_in_buf(MAX_BUF_SIZE),
      _in_to_out_stream(&_in_to_out_buf),
      _out_to_in_stream(&_out_to_in_buf) {
  if (_in->holder() == nullptr)
    _in->holder() = this;
}

ConnHolder::~ConnHolder() {
  _in->strategy()->stop();
  if (_out)
    _out->strategy()->stop();
}

void ConnHolder::toInRead() {
  _in->toInRead(shared_from_this());
}

void ConnHolder::toInWrite() {
  _in->toInWrite(shared_from_this());
}

void ConnHolder::toOutRead() {
  _out->toOutRead(shared_from_this());
}

void ConnHolder::toOutWrite() {
  _out->toOutWrite(shared_from_this());
}

void ConnHolder::inRead() {
  _in->inRead(shared_from_this());
}

void ConnHolder::inWrite() {
  _in->inWrite(shared_from_this());
}

void ConnHolder::outRead() {
  _out->outRead(shared_from_this());
}

void ConnHolder::outWrite() {
  _out->outWrite(shared_from_this());
}

void ConnHolder::dial() {
  _out = router->pickRoute(this, _remote);
  if (_out)
    _out->dial(shared_from_this());
}

void ConnHolder::closeIn(CloseType type) {
  if (_in)
    _in->closeMe(type);
}

void ConnHolder::closeOut(CloseType type) {
  if (_out)
    _out->closeMe(type);
}

void ConnHolder::closeAll() {
  closeIn(CloseType::both);
  closeOut(CloseType::both);
}
