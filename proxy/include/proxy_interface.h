#ifndef NETCAT_PROXY_INTERFACE_H
#define NETCAT_PROXY_INTERFACE_H

#include <cstdlib>
#include <cstdint>

#include "types.h"


/**
 * @brief ConnHandler is a interface of Listener and Dialer
 *
 * Every Connection must have a reference of ConnHandler.
 * ConnHandler is usually implemented as State design pattern.
 * It change ConnHandler in Connection so that make Connection
 * change its behavior by calling interface defined by ConnHandler.
 *
 */
class ConnHandler {
public:
    virtual ssize_t parseInRead(conn_p conn, const uint8_t *msg, ssize_t sz) { return sz; };
    virtual ssize_t parseInWrite(conn_p conn, const uint8_t *msg, ssize_t sz) { return sz; };
    virtual ssize_t parseOutRead(conn_p conn, const uint8_t *msg, ssize_t sz) { return sz; };
    virtual ssize_t parseOutWrite(conn_p conn, const uint8_t *msg, ssize_t sz) { return sz; };

protected:
    virtual void setHandler(conn_p conn, ConnHandler *handler);
};

/**
 * @brief Listener should parse connection information for transfer task to Dialer.
 *
 * Listener parses data of remote address, remote port,
 * connection type(TCP, UPD, ...) and address type(IPv4, IPv6, Domain)
 * and stores them back to Connection.
 * Listener are usually implemented as State design pattern.
 * In its last step it must call conn->transferForDialer(Dialer *)
 * for transferring the task of transferring data to specified Dialer.
 *
 * Listener usually does encode/encrypt and decode/decrypt works.
 *
 */
class Listener : public ConnHandler {

};

/**
 * @brief Dialer takes over transferring data task from Listener.
 *
 * Dialer make a connection between local machine and remote address
 * according to data in Connection. It must be started by calling
 * doDial(tcp_p). If connection is established, doDial() function
 * will open pipe between local and remote by calling conn->inRead()
 * and conn->outRead.
 *
 * Dialer usually does encode/encrypt and decode/decrypt works.
 *
 */
class Dialer : public ConnHandler {
public:
    virtual void doDial(conn_p conn) = 0;
};


#endif //NETCAT_PROXY_INTERFACE_H
