#ifndef NETCAT_CONNECTION_H
#define NETCAT_CONNECTION_H

#include <asio.hpp>

#include "common/net.h"
#include "common/types.h"
#include "proxy/proxy_interface.h"

using namespace asio;

/**
 * @brief InConn does thing related to in socket
 *
 * InConn parses remote information for OutConn.
 * It does in socket handshaking after created.
 * It also does real data transfer to in socket
 * after remote connection established.
 *
 */
class InConn {
public:
    InConn(ConnHolder *holder, proxy::AcceptStrategy *strategy)
            : _holder(holder), _strategy(strategy) {}
    virtual ~InConn() = default;

    virtual void toInRead(holder_p holder);
    virtual void toInWrite(holder_p holder);
    virtual void inRead(holder_p holder) = 0;
    virtual void inWrite(holder_p holder) = 0;

    ConnHolder *&holder();
    proxy::AcceptStrategy *&strategy();

    virtual void closeMe(CloseType type) = 0;

protected:
    ConnHolder *_holder;
    proxy::AcceptStrategy *_strategy;
};

/**
 * @brief OutConn does things related to out socket.
 *
 * OutConn picks route by remote address the InConn parsed.
 * Then passes real connecting job to its dial() method.
 * After that connection established it does data transfer job
 * cooperating with InConn.
 *
 */
class OutConn {
public:
    OutConn(ConnHolder *holder, proxy::DialStrategy *strategy)
            : _holder(holder), _strategy(strategy) {}
    virtual ~OutConn() = default;

    virtual void toOutRead(holder_p holder);
    virtual void toOutWrite(holder_p holder);
    virtual void outRead(holder_p holder) = 0;
    virtual void outWrite(holder_p holder) = 0;

    ConnHolder *&holder();
    proxy::DialStrategy *&strategy();

    virtual void closeMe(CloseType type) = 0;
    virtual void dial(holder_p holder/* route info */) = 0;

protected:
    ConnHolder *_holder;
    proxy::DialStrategy *_strategy;
};

// below are inline methods' definitions.
inline ConnHolder *&InConn::holder() {
    return _holder;
}

inline proxy::AcceptStrategy *&InConn::strategy() {
    return _strategy;
}

inline ConnHolder *&OutConn::holder() {
    return _holder;
}

inline proxy::DialStrategy *&OutConn::strategy() {
    return _strategy;
}

#endif