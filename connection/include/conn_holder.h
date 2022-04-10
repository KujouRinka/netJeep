#ifndef NETCAT_CONN_HOLDER_H
#define NETCAT_CONN_HOLDER_H

#include <iostream>
#include <memory>
#include <asio.hpp>
#include <utility>

#include "net.h"
#include "types.h"

/**
 * @brief ConnHolder holds connection for in and out socket.
 *
 * ConnHolder is the key class for connection. It holds a
 * InConn from Listener and instantiates OutConn via information
 * parsed from InConn.
 * ConnHolder holds common data both InConn and OutConn need.
 * It plays part as a mediator for data transfer between
 * InConn and OutConn and keeps smart pointers of them
 * to ensure their memory spaces keeping valid before connection closed.
 *
 * @attention ConnHolder MUST make sure its self be hold while async call waiting.
 *
 */
class ConnHolder : public enable_shared_from_this<ConnHolder> {
public:
    ConnHolder(asio::io_context &ctx, in_p in, long long id);
    ~ConnHolder();

    void start();
    void inRead();
    void inWrite();
    void outRead();
    void outWrite();

    std::iostream &IOBufStream();
    std::iostream &OIBufStream();
    asio::streambuf &IOBuf();
    asio::streambuf &OIBuf();
    size_t IOMaxRDTo();
    size_t IOMaxWRFrom();
    size_t OIMaxRDTo();
    size_t OIMaxWRFrom();

    asio::io_context &getCtx();
    long long id();
    NetAddress &remote();

    void dial();
    void closeIn(CloseType type);
    void closeOut(CloseType type);
    void closeAll();

private:
    asio::io_context &_ctx;
    long long _id;

    asio::streambuf _in_to_out_buf;
    asio::streambuf _out_to_in_buf;
    std::iostream _in_to_out_stream;
    std::iostream _out_to_in_stream;
    NetAddress _remote;

    in_p _in;
    out_p _out;
};

// below are inline methods' definitions.
inline void ConnHolder::start() {
    inRead();
}

inline std::iostream &ConnHolder::IOBufStream() {
    return _in_to_out_stream;
}

inline std::iostream &ConnHolder::OIBufStream() {
    return _out_to_in_stream;
}

inline asio::streambuf &ConnHolder::IOBuf() {
    return _in_to_out_buf;
}

inline asio::streambuf &ConnHolder::OIBuf() {
    return _out_to_in_buf;
}

inline size_t ConnHolder::IOMaxRDTo() {
    return _in_to_out_buf.max_size() - _in_to_out_buf.size();
}

inline size_t ConnHolder::IOMaxWRFrom() {
    return _in_to_out_buf.size();
}

inline size_t ConnHolder::OIMaxRDTo() {
    return _out_to_in_buf.max_size() - _out_to_in_buf.size();
}

inline size_t ConnHolder::OIMaxWRFrom() {
    return _out_to_in_buf.size();
}

inline asio::io_context &ConnHolder::getCtx() {
    return _ctx;
}

inline long long ConnHolder::id() {
    return _id;
}

inline NetAddress &ConnHolder::remote() {
    return _remote;
}

#endif