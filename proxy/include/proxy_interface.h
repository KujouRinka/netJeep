#ifndef NETCAT_PROXY_INTERFACE_H
#define NETCAT_PROXY_INTERFACE_H

#include <cstdlib>
#include <cstdint>

#include "types.h"

class ConnHolder;

/**
 * @brief AcceptStrategy parses out necessary information for DialStrategy.
 *
 * AcceptStrategy parses information of remote address, remote port,
 * connection type(TCP, UPD, ...) and address type(IPv4, IPv6, Domain)
 * and stores them into ConnHolder::_remote.
 * AcceptStrategy is usually implemented as State design pattern.
 * In its last step it must call holder->dial() to inform ConnHolder
 * all information needed has been parsed.
 *
 * AcceptStrategy usually does encode/encrypt and decode/decrypt works.
 *
 */
class AcceptStrategy {
public:
    virtual ssize_t toInRead(ConnHolder *holder, InConn *in);
    virtual ssize_t toInWrite(ConnHolder *holder, InConn *in);
    /**
     * onInRead will be called in callback of async read of in socket.
     * It calls holder->toOutWrite() when all jobs done.
     *
     * @param holder ConnHolder which holds owner of AcceptStrategy.
     * @param in InConn which holds this AcceptStrategy.
     * @return returns -1 if operation failed.
     */
    virtual ssize_t onInRead(ConnHolder *holder, InConn *in) = 0;

    /**
     * onInWrite will be called in callback of async write of in socket.
     * It calls holder->toOutRead() when all jobs done.
     *
     * @param holder ConnHolder which holds owner of AcceptStrategy.
     * @param in InConn which holds this AcceptStrategy.
     * @return returns -1 if operation failed.
     */
    virtual ssize_t onInWrite(ConnHolder *holder, InConn *in) = 0;

    virtual void stop();
};


/**
 * @brief DialStrategy dials to remote by information in ConnHolder::_remote.
 *
 * DialStrategy makes a connection between local machine and remote address
 * according to address information stored in ConnHolder.
 * DialStrategy is implemented as State design pattern, but usually
 * has no state transition.
 *
 * DialStrategy usually does encode/encrypt and decode/decrypt works.
 *
 */
class DialStrategy {
public:
    virtual ssize_t toOutRead(ConnHolder *holder, OutConn *out);
    virtual ssize_t toOutWrite(ConnHolder *holder, OutConn *out);
    /**
     * onOutRead will be called in callback of async read of out socket.
     * It calls holder->toInWrite() when all jobs done.
     *
     * @param holder ConnHolder which holds owner of AcceptStrategy.
     * @param in InConn which holds this AcceptStrategy.
     * @return returns -1 if operation failed.
     */
    virtual ssize_t onOutRead(ConnHolder *holder, OutConn *out) = 0;


    /**
     * onOutWrite will be called in callback of async write of out socket.
     * It calls holder->toInRead() when all jobs done.
     *
     * @param holder ConnHolder which holds owner of AcceptStrategy.
     * @param in InConn which holds this AcceptStrategy.
     * @return returns -1 if operation failed.
     */
    virtual ssize_t onOutWrite(ConnHolder *holder, OutConn *out) = 0;

    virtual void stop();
};


#endif //NETCAT_PROXY_INTERFACE_H
