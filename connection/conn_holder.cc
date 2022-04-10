#include "conn_holder.h"

#include "tcp.h"
#include "direct.h"

extern std::mutex closed_count_lock;
extern long long conn_closed;

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
    closed_count_lock.lock();
    ++conn_closed;
    closed_count_lock.unlock();
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
    // TODO: route pick
    switch (_remote.conn_type()) {
        case ConnType::TCP:
            // TODO: pick route for connection
            _out = make_shared<TCPOut>(this, DialDirect::startStat());
            _out->dial(shared_from_this());
            break;
        case ConnType::UDP:
            // TODO: Kujourinka
            break;
        case ConnType::RAW:
            // TODO: Kujourinka
            break;
    }
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
