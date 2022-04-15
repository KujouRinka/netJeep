#ifndef NETJEEP_PROXY_MANAGER_H
#define NETJEEP_PROXY_MANAGER_H

#include <vector>
#include <unordered_map>
#include <utility>

#include "common/types.h"

using namespace std;

namespace asio {
    class io_context;
}

using accept_call = function<void()>;
class AcceptorManager {
public:
    AcceptorManager() = delete;
    static void init();
    static void initWithContext(asio::io_context &ctx);

    static void addAcceptable(void (*ac)());
    static AcceptorManager *getAcceptor();
    static void acceptAll();

private:
    static asio::io_context *_ctx;
    static vector<accept_call> _acceptor_holder;
};


using dial_core_builder = function<dial_core(ConnHolder *)>;
// using dial_core = std::pair<proxy::DialStrategy *, NetAddress *>;
class DialerManager {
public:
    DialerManager() = delete;
    static void init();

    static void addDialerBuilder(string tag, dial_core_builder builder);
    static dial_core getDialerForHolder(const string &tag, ConnHolder *holder);
    static dial_core getDefaultDialerForHolder(ConnHolder *holder);

private:
    static unordered_map<string, dial_core_builder> _tagged_dialer;
    static dial_core_builder _default_builder;
};

#endif
