#include "tcp.h"

#include "conn_holder.h"

using namespace std;

extern int conn_closed;
extern std::mutex closed_count_lock;

TCPIn::TCPIn(tcp_sock_p p, ConnHolder *holder, proxy::AcceptStrategy *strategy)
        : _in_sock(p), InConn(holder, strategy) {}

void TCPIn::toInRead(holder_p holder) {
    _strategy->toInRead(_holder, this);
}

void TCPIn::toInWrite(holder_p holder) {
    _strategy->toInWrite(_holder, this);
}

void TCPIn::inRead(holder_p holder) {
    _in_sock->async_read_some(
            holder->IOBuf().prepare(holder->IOMaxRDTo()),
            [this, holder](const error_code err, ssize_t sz) {
                // in_on_read
                holder->IOBuf().commit(sz);
                if (err) {
                    cout << _holder->id() << ": in read: " << err.message() << endl;
                    if (err != error::eof) {
                        _holder->closeAll();
                    } else {
                        // cap->closeMe(CloseType::read);
                        _holder->closeOut(CloseType::write);
                    }
                    return;
                }
                _strategy->onInRead(_holder, this);
            }
    );
}

void TCPIn::inWrite(holder_p holder) {
    _in_sock->async_write_some(
            holder->OIBuf().data(),
            [this, holder](const error_code err, ssize_t sz) {
                // on_in_write
                holder->OIBuf().consume(sz);
                if (err) {
                    cout << _holder->id() << ": in write: " << err.message() << endl;
                    _holder->closeAll();
                    return;
                }
                _strategy->onInWrite(_holder, this);
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

TCPOut::TCPOut(ConnHolder *holder, proxy::DialStrategy *strategy)
        : _out_sock(new tcp_sock(holder->getCtx())),
          OutConn(holder, strategy), _resolver(holder->getCtx()) {}

void TCPOut::toOutRead(holder_p holder) {
    _strategy->toOutRead(_holder, this);
}

void TCPOut::toOutWrite(holder_p holder) {
    _strategy->toOutWrite(_holder, this);
}

void TCPOut::outRead(holder_p holder) {
    _out_sock->async_read_some(
            holder->OIBuf().prepare(holder->OIMaxRDTo()),
            [this, holder](const error_code err, ssize_t sz) {
                // out_on_read
                holder->OIBuf().commit(sz);
                if (err) {
                    cout << _holder->id() << ": out read: " << err.message() << endl;
                    if (err != error::eof) {
                        _holder->closeAll();
                    } else {
                        // cap->closeMe(CloseType::read);
                        _holder->closeIn(CloseType::write);
                    }
                    return;
                }
                _strategy->onOutRead(_holder, this);
            }
    );
}

void TCPOut::outWrite(holder_p holder) {
    _out_sock->async_write_some(
            holder->IOBuf().data(),
            [this, holder](const error_code err, ssize_t sz) {
                // on_out_write
                holder->IOBuf().consume(sz);
                if (err) {
                    cout << _holder->id() << ": out write: " << err.message() << endl;
                    _holder->closeAll();
                    return;
                }
                _strategy->onOutWrite(_holder, this);
            }
    );
}

void TCPOut::dial(holder_p holder) {
    cout << _holder->id() << ": TCP://"
         << _holder->remote().addr() << ":"
         << _holder->remote().port() << " connecting" << endl;
    cout << _holder->id() << ": TCP://"
         << _holder->dialAddress().addr() << ":"
         << _holder->dialAddress().port() << " dialing" << endl;
    // TODO: route pick
    if (_holder->dialAddress().addr_type() == AddrType::Domain) {
        _resolver.async_resolve(
                _holder->dialAddress().addr(), to_string(_holder->dialAddress().port()),
                [this, cap = holder](const error_code err, ip::tcp::resolver::iterator it) {
                    if (err)
                        return;
                    dialHelper(*it, cap);
                }
        );
    } else {
        dialHelper(
                ip::tcp::endpoint(
                        ip::address::from_string(_holder->dialAddress().addr()),
                        _holder->dialAddress().port()
                ),
                holder
        );
    }
}

void TCPOut::dialHelper(ip::tcp::endpoint ep, holder_p holder) {
    _out_sock->async_connect(
            ep, [this, cap = std::move(holder)](const error_code err) {
                if (err) {
                    cout << cap->id() << ": " << cap->dialAddress().addr() << ":"
                         << cap->dialAddress().port() << " connect error" << endl;
                    return;
                }
                cout << cap->id() << ": TCP://" << cap->dialAddress().addr() << ":"
                     << cap->dialAddress().port() << " connected" << endl;
                cap->toOutRead();
                cap->toInRead();
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
