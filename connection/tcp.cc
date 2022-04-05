#include "tcp.h"

#include <iostream>

#include "direct.h"
#include "buffer.h"

using namespace std;

extern int conn_closed;
extern std::mutex closed_count_lock;

TCPIn::~TCPIn() {
    closed_count_lock.lock();
    ++conn_closed;
    closed_count_lock.unlock();
    // InConn::closeAll();
}

void TCPIn::inRead(conn_p holder) {
    _in_sock->async_read_some(
            buffer(inToOutBuf()->get(), BUF_CAP),
            [cap = shared_from_this(), holder](const error_code err, ssize_t sz) {
                // in_on_read
                if (err) {
                    cout << cap->id() << ": in read: " << err.message() << endl;
                    if (err != error::eof) {
                        cap->closeAll();
                    } else {
                        cap->closeMe(CloseType::read);
                        cap->closeOtherSide(CloseType::write);
                    }
                    return;
                }
                cap->inToOutBuf()->resize(sz);
                cap->connHandler()->parseInRead(cap, holder);
            }
    );
}

void TCPIn::inWrite(conn_p holder) {
    async_write(
            *_in_sock,
            buffer(outToInBuf()->get(), outToInBuf()->size()),
            [cap = shared_from_this(), holder](const error_code err, ssize_t sz) {
                // on_in_write
                if (err) {
                    cout << cap->id() << " in write: " << err.message() << endl;
                    cap->closeAll();
                    return;
                }
                cap->connHandler()->parseInWrite(cap, holder);
            }
    );
}

void TCPIn::closeMe(CloseType type) {
    if (!_in_sock->is_open())
        return;
    error_code err;
    switch (type) {
        case CloseType::read:
            _in_sock->shutdown(ip::tcp::socket::shutdown_receive, err);
            break;
        case CloseType::write:
            _in_sock->shutdown(ip::tcp::socket::shutdown_send, err);
            break;
        case CloseType::both:
            _in_sock->cancel(err);
            _in_sock->close(err);
            break;
    }
}

TCPOut::~TCPOut() noexcept {
    // all tasks done by ConnIn
}

void TCPOut::outRead(conn_p holder) {
    _out_sock->async_read_some(
            buffer(outToInBuf()->get(), BUF_CAP),
            [cap = shared_from_this(), holder](const error_code err, ssize_t sz) {
                // out_on_read
                if (err) {
                    cout << cap->id() << " out read: " << err.message() << endl;
                    if (err != error::eof) {
                        cap->closeAll();
                    } else {
                        cap->closeMe(CloseType::read);
                        cap->closeOtherSide(CloseType::write);
                    }
                    return;
                }
                cap->outToInBuf()->resize(sz);
                cap->connHandler()->parseOutRead(cap, holder);
            }
    );
}

void TCPOut::outWrite(conn_p holder) {
    async_write(
            *_out_sock,
            buffer(inToOutBuf()->get(), inToOutBuf()->size()),
            [cap = shared_from_this(), holder](const error_code err, ssize_t sz) {
                // on_out_write
                if (err) {
                    cout << cap->id() << " out write: " << err.message() << endl;
                    cap->closeAll();
                    return;
                }
                cap->connHandler()->parseOutWrite(cap, holder);
            }
    );
}

void TCPOut::dial(conn_p in_holder) {
    cout << id() << " " << remote().addr() << ":" << remote().port() << " connecting" << endl;
    // TODO: route pick
    in_holder->connHandler() = DirectDialer::instance();
    connHandler() = DirectDialer::instance();
    if (remote().addr_type() == AddrType::Domain) {
        _resolver.async_resolve(
                remote().addr(), to_string(remote().port()),
                [this, cap = shared_from_this(), in_holder](const error_code err, ip::tcp::resolver::iterator it) {
                    if (err)
                        return;
                    dialHelper(*it, in_holder);
                }
        );
    } else {
        dialHelper(ip::tcp::endpoint(ip::address::from_string(remote().addr()), remote().port()), in_holder);
    }
}

void TCPOut::dialHelper(ip::tcp::endpoint ep, conn_p in_holder) {
    holder()->holder() = this;
    _out_sock->async_connect(
            ep, [cap = shared_from_this(), in_holder](const error_code err) {
                if (err) {
                    cout << cap->id() << " " << cap->remote().addr() << ":"
                         << cap->remote().port() << " connect error" << endl;
                    return;
                }
                cout << cap->id() << " " << cap->remote().addr() << ":"
                     << cap->remote().port() << " connected" << endl;
                cap->connHandler() = DirectDialer::instance();
                cap->outRead(in_holder);
                in_holder->inRead(cap);
            }
    );
}

void TCPOut::closeMe(CloseType type) {
    if (!_out_sock->is_open())
        return;
    error_code err;
    switch (type) {
        case CloseType::read:
            _out_sock->shutdown(ip::tcp::socket::shutdown_receive, err);
            break;
        case CloseType::write:
            _out_sock->shutdown(ip::tcp::socket::shutdown_send, err);
            break;
        case CloseType::both:
            _out_sock->cancel(err);
            _out_sock->close(err);
            break;
    }
}
