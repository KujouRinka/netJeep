#ifndef NETJEEP_ROUTER_DEFAULT_H
#define NETJEEP_ROUTER_DEFAULT_H

#include "interface.h"

class ConnHolder;
class NetAddress;

class DefaultRouter : public Router {
 public:
  out_p pickRoute(ConnHolder *holder, NetAddress &remote) override;
};

#endif