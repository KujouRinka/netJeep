#ifndef NETJEEP_ROUTER_INTERFACE_H
#define NETJEEP_ROUTER_INTERFACE_H

#include "common/types.h"

namespace proxy {
    class DialStrategy;
}
class NetAddress;
class ConnHolder;

class Router {
public:
    virtual out_p pickRoute(ConnHolder *holder, NetAddress &remote) = 0;
};

#endif
