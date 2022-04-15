#include "default.h"

#include "common/net.h"
#include "connection/tcp.h"
#include "proxy_manager/manager.h"

out_p DefaultRouter::pickRoute(ConnHolder *holder, NetAddress &remote) {
    auto type = remote.conn_type();
    if (type == ConnType::TCP) {
        auto core = DialerManager::getDefaultDialerForHolder(holder);
        if (core.second == nullptr)
            core.second = &remote;
        return make_shared<TCPOut>(holder, core);
    } else if (type == ConnType::UDP) {
        // TODO: UDP implementation
    }
    return nullptr;
}
