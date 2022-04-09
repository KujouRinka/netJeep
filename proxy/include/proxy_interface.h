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
    /**
     * onInRead will be called in callback of async read of in socket.
     * It calls holder->outWrite() when all jobs done.
     *
     * @param holder ConnHolder which holds owner of AcceptStrategy.
     * @param in InConn which holds this AcceptStrategy.
     * @return returns -1 if operation failed.
     */
    virtual ssize_t onInRead(ConnHolder *holder, InConn *in) = 0;

    /**
     * onInWrite will be called in callback of async write of in socket.
     * It calls holder->outRead() when all jobs done.
     *
     * @param holder ConnHolder which holds owner of AcceptStrategy.
     * @param in InConn which holds this AcceptStrategy.
     * @return returns -1 if operation failed.
     */
    virtual ssize_t onInWrite(ConnHolder *holder, InConn *in) = 0;
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

    /**
     * onOutRead will be called in callback of async read of out socket.
     * It calls holder->inWrite() when all jobs done.
     *
     * @param holder ConnHolder which holds owner of AcceptStrategy.
     * @param in InConn which holds this AcceptStrategy.
     * @return returns -1 if operation failed.
     */
    virtual ssize_t onOutRead(ConnHolder *holder, OutConn *out) = 0;


    /**
     * onOutWrite will be called in callback of async write of out socket.
     * It calls holder->inRead() when all jobs done.
     *
     * @param holder ConnHolder which holds owner of AcceptStrategy.
     * @param in InConn which holds this AcceptStrategy.
     * @return returns -1 if operation failed.
     */
    virtual ssize_t onOutWrite(ConnHolder *holder, OutConn *out) = 0;
};


#endif //NETCAT_PROXY_INTERFACE_H
