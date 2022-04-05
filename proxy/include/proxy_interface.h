#ifndef NETCAT_PROXY_INTERFACE_H
#define NETCAT_PROXY_INTERFACE_H

#include <cstdlib>
#include <cstdint>

#include "types.h"

class Buffer;

/**
 * @brief ConnHandler is a interface of Authenticator and Dialer
 *
 * Every BackgroundConn must have a reference of ConnHandler.
 * ConnHandler is usually implemented as State design pattern.
 * It change ConnHandler in BackgroundConn so that make BackgroundConn
 * change its behavior by calling interface defined by ConnHandler.
 *
 */
class ConnHandler {
public:
    /**
     * @brief be called after read from in socket.
     * @param conn who calls this method
     * @param holder connection to be prevented to be freed while async wait
     */
    virtual ssize_t parseInRead(conn_p conn, conn_p holder);

    /**
     * @brief be called after write to in socket.
     * @param conn who calls this method
     * @param holder connection to be prevented to be freed while async wait
     */
    virtual ssize_t parseInWrite(conn_p conn, conn_p holder);

    /**
     * @brief be called after read from out socket.
     * @param conn who calls this method
     * @param holder connection to be prevented to be freed while async wait
     */
    virtual ssize_t parseOutRead(conn_p conn, conn_p holder);

    /**
     * @brief be called after write to out socket.
     * @param conn who calls this method
     * @param holder connection to be prevented to be freed while async wait
     */
    virtual ssize_t parseOutWrite(conn_p conn, conn_p holder);
};

/**
 * @brief Authenticator should parse connection information for transfer task to Dialer.
 *
 * Authenticator parses data of remote address, remote port,
 * connection type(TCP, UPD, ...) and address type(IPv4, IPv6, Domain)
 * and stores them back to BackgroundConn.
 * Authenticator are usually implemented as State design pattern.
 * In its last step it must call conn->transferForDialer(Dialer *)
 * for transferring the task of transferring data to specified Dialer.
 *
 * Authenticator usually does encode/encrypt and decode/decrypt works.
 *
 */
class Authenticator : public ConnHandler {

};

/**
 * @brief Dialer takes over transferring data task from Authenticator.
 *
 * Dialer make a connection between local machine and remote address
 * according to data in BackgroundConn. It must be started by calling
 * doDial(tcp_p). If connection is established, doDial() function
 * will open pipe between local and remote by calling conn->inRead()
 * and conn->outRead.
 *
 * Dialer usually does encode/encrypt and decode/decrypt works.
 *
 */
class Dialer : public ConnHandler {

};


#endif //NETCAT_PROXY_INTERFACE_H
