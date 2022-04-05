#include "connection.h"

#include <iostream>
#include <memory>

#include "tcp.h"

InConn::~InConn() {
    delete _in_to_out_buf;
    delete _out_to_in_buf;
}

void InConn::start() {
    inRead(nullptr);
}

void InConn::outRead(conn_p holder) {
    holder->outRead(shared_from_this());
}

void InConn::outWrite(conn_p holder) {
    holder->outWrite(shared_from_this());
}

Buffer *InConn::inToOutBuf() {
    return _in_to_out_buf;
}

Buffer *InConn::outToInBuf() {
    return _out_to_in_buf;
}

io_context &InConn::getCtx() {
    return _ctx;
}

NetAddress &InConn::remote() {
    return _remote;
}

void InConn::closeOtherSide(CloseType type) {
    if (_out_holder)
        _out_holder->closeMe(type);
}

void InConn::closeAll() {
    cout << id() << ": connection for: " << remote().addr() << ":" << remote().port() << " CLOSED" << endl;
    closeMe(CloseType::both);
    closeOtherSide(CloseType::both);
}

long long InConn::id() {
    return _id;
}

ConnHandler *&InConn::connHandler() {
    return _conn_handler;
}

BaseConn *&InConn::holder() {
    return _out_holder;
}

void InConn::dial(conn_p base) {
    if (remote().conn_type() == ConnType::TCP) {
        std::make_shared<TCPOut>(shared_from_this())->start();
    } else if (remote().conn_type() == ConnType::UDP) {
        // TODO: add UDP support
        closeAll();
    } else {
        closeAll();
    }
}

// TODO: DON'T use dynamic cast
void OutConn::start() {
    auto p = dynamic_cast<InConn *>(_in_holder);
    if (p)
        dial(p->shared_from_this());
}

void OutConn::inRead(conn_p holder) {
    holder->inRead(shared_from_this());
}

void OutConn::inWrite(conn_p holder) {
    holder->inWrite(shared_from_this());
}

Buffer *OutConn::inToOutBuf() {
    return _in_holder->inToOutBuf();
}

Buffer *OutConn::outToInBuf() {
    return _in_holder->outToInBuf();
}

io_context &OutConn::getCtx() {
    return _ctx;
}

NetAddress &OutConn::remote() {
    return _in_holder->remote();
}

void OutConn::closeOtherSide(CloseType type) {
    if (_in_holder)
        _in_holder->closeMe(type);
}

void OutConn::closeAll() {
    cout << id() << ": connection for: " << remote().addr() << ":" << remote().port() << " CLOSED" << endl;
    closeMe(CloseType::both);
    closeOtherSide(CloseType::both);
}

long long OutConn::id() {
    return _in_holder->id();
}

ConnHandler *&OutConn::connHandler() {
    return _conn_handler;
}

BaseConn *&OutConn::holder() {
    return _in_holder;
}
