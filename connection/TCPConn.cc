#include "TCPConn.h"

#include "proxy_interface.h"
#include <iostream>

extern long long conn_closed;
extern mutex closed_count_lock;

RealConn::RealConn(io_context &ctx, tcp_sock_p sock_p, ConnHandler *handler, long long conn_id)
        : _ctx(ctx), _in_sock(sock_p), _out_sock(new tcp_sock(ctx)),
          _handler(handler), _id(conn_id), _resolver(ctx) {}

RealConn::~RealConn() {
    closed_count_lock.lock();
    ++conn_closed;
    closed_count_lock.unlock();
    close();
    cout << _id << ": connection for: " << _remote_addr << ":" << _remote_port << " CLOSED" << endl;
}

void RealConn::inRead() {
    _in_sock->async_read_some(
            buffer(_in_to_out_buf, BUF_SIZE),
            [cap = shared_from_this()](const error_code err, ssize_t sz) {
                // in_on_read
                if (err) {
                    cout << cap->_id << ": in read: " << err.message() << endl;
                    if (err != error::eof) {
                        cap->close();
                    } else {
                        error_code ec;
                        cap->_in_sock->shutdown(ip::tcp::socket::shutdown_receive, ec);
                        cap->_out_sock->shutdown(ip::tcp::socket::shutdown_send, ec);
                    }
                    return;
                }
                cap->_out_write_len = sz;
                cap->_handler->parseInRead(cap, cap->_in_to_out_buf, sz);
            }
    );
}

void RealConn::inWrite() {
    async_write(
            *_in_sock,
            buffer(_out_to_in_buf, _in_write_len),
            [cap = shared_from_this()](const error_code err, ssize_t sz) {
                // on_in_write
                if (err) {
                    cout << cap->_id << " in write: " << err.message() << endl;
                    cap->close();
                    return;
                }
                cap->_handler->parseInWrite(cap, cap->_out_to_in_buf, sz);
            }
    );
}

void RealConn::outRead() {
    _out_sock->async_read_some(
            buffer(_out_to_in_buf, BUF_SIZE),
            [cap = shared_from_this()](const error_code err, ssize_t sz) {
                // out_on_read
                if (err) {
                    cout << cap->_id << " out read: " << err.message() << endl;
                    if (err != error::eof) {
                        cap->close();
                    } else {
                        error_code ec;
                        cap->_out_sock->shutdown(ip::tcp::socket::shutdown_receive, ec);
                        cap->_in_sock->shutdown(ip::tcp::socket::shutdown_send, ec);
                    }
                    return;
                }
                cap->_in_write_len = sz;
                cap->_handler->parseOutRead(cap, cap->_out_to_in_buf, sz);
            }
    );
}

void RealConn::outWrite() {
    async_write(
            *_out_sock,
            buffer(_in_to_out_buf, _out_write_len),
            [cap = shared_from_this()](const error_code err, ssize_t sz) {
                // on_out_write
                if (err) {
                    cout << cap->_id << " out write: " << err.message() << endl;
                    cap->close();
                    return;
                }
                cap->_handler->parseOutWrite(cap, cap->_in_to_out_buf, sz);
            }
    );
}

void RealConn::transferToDialer(Dialer *dialer) {
    setHandler(dialer);
    dialer->doDial(shared_from_this());
}

void RealConn::close() {
    if (_in_sock->is_open()) {
        _in_sock->cancel();
        _in_sock->close();
    }
    if (_out_sock->is_open()) {
        _out_sock->cancel();
        _out_sock->close();
    }
}

void RealConn::setHandler(ConnHandler *handler) {
    _handler = handler;
}
