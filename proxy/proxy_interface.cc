#include "proxy_interface.h"

#include "TCPConn.h"

void ConnHandler::setHandler(conn_p conn, ConnHandler *handler) {
    conn->setHandler(handler);
}
