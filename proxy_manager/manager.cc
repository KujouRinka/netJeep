#include "manager.h"

#include <memory>
#include <functional>

#include "asio.hpp"

#include "connection/protocol.h"
#include "connection/conn_holder.h"
#include "proxy/proxy.h"
#include "config/config.h"

using namespace std;
using namespace asio;

extern long long conn_opened;
extern mutex closed_count_lock;
extern long long conn_closed;
extern Config::Config *global_config;

asio::io_context *AcceptorManager::_ctx = new io_context;
vector<acceptFunc> AcceptorManager::_acceptor_holder;
unordered_map<string, dialCoreBuilder> DialerManager::_tagged_dialer;
dialCoreBuilder DialerManager::_default_builder;

void AcceptorManager::init() {
    try {
        for (Config::Acceptor &ac: global_config->acceptors) {
            if (ac.protocol == "socks")
                addAcceptable(ac.tag, proxy::Socks::acceptFuncFromConfig(*_ctx, ac));
            if (ac.protocol == "aes_128_cfb_test")
                addAcceptable(ac.tag, proxy::AES128::acceptFuncFromConfig(*_ctx, ac));
        }
    } catch (exception &e) {
        cerr << "init config file error: in acceptors" << endl;
        cerr << e.what() << endl;
        exit(1);
    }
}

void AcceptorManager::initWithContext(asio::io_context &ctx) {
    delete _ctx;
    _ctx = &ctx;
    init();
}

void AcceptorManager::addAcceptable(const string &tag, acceptFunc ac) {
    cout << "acceptor: " << tag << " added" << endl;
    _acceptor_holder.emplace_back(std::move(ac));
}

AcceptorManager *AcceptorManager::getAcceptor() {
    return nullptr;
}

void AcceptorManager::acceptAll() {
    for (auto &accept: _acceptor_holder)
        accept();
    _ctx->run();
}

void DialerManager::init() {
    try {
        for (Config::Dialer &d: global_config->dialers) {
            if (d.protocol == "direct")
                addDialerBuilder(d.tag, proxy::Direct::dialBuilderFromConfig(d));
            if (d.protocol == "aes_128_cfb_test")
                addDialerBuilder(d.tag, proxy::AES128::dialBuilderFromConfig(d));
        }
        _default_builder = _tagged_dialer[global_config->dialers[0].tag];
    } catch (exception &e) {
        cerr << "init config file error: in dialers" << endl;
        cerr << e.what() << endl;
        exit(1);
    }
}

void DialerManager::addDialerBuilder(string tag, dialCoreBuilder builder) {
    if (tag.empty())
        return;
    cout << "dialer: " << tag << " added" << endl;
    _tagged_dialer[std::move(tag)] = std::move(builder);
}

dialCore DialerManager::getDialCoreForHolder(const string &tag, ConnHolder *holder) {
    if (_tagged_dialer.count(tag))
        return _tagged_dialer[tag](holder);
    return getDefaultDialerForHolder(holder);
}

dialCore DialerManager::getDefaultDialerForHolder(ConnHolder *holder) {
    return _default_builder(holder);
}
