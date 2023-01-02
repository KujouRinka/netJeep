#ifndef NETJEEP_PROXY_TYPES_H
#define NETJEEP_PROXY_TYPES_H

#include <functional>

#include "common/types.h"

using acceptFunc = std::function<void()>;
using dialCore = std::pair<proxy::DialStrategy *, NetAddress *>;
using dialCoreBuilder = std::function<dialCore(ConnHolder * )>;

#endif