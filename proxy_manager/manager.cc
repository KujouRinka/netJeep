#include "manager.h"

#include <memory>
#include <functional>

#include "asio.hpp"

#include "connection/protocol.h"
#include "connection/conn_holder.h"
#include "proxy/proxy.h"

using namespace std;
using namespace asio;

extern long long conn_opened;
extern mutex closed_count_lock;
extern long long conn_closed;

asio::io_context *AcceptorManager::_ctx = new io_context;
vector<accept_call> AcceptorManager::_acceptor_holder;
unordered_map<string, dial_core_builder> DialerManager::_tagged_dialer;
dial_core_builder DialerManager::_default_builder;

void acceptTCP(ip::tcp::acceptor *ac_sock, io_context &ctx) {
    tcp_sock_p sp(new tcp_sock(ctx));
    ac_sock->async_accept(*sp, [sp, ac_sock, &ctx](const error_code err) {
        if (!err) {
            ++conn_opened;
            make_shared<ConnHolder>(
                    ctx, make_shared<TCPIn>(sp, nullptr, proxy::Socks::Acceptor::startStat()),
                    conn_opened
            )->start();
        }
        acceptTCP(ac_sock, ctx);
    });
}

void AcceptorManager::init() {
    auto ac_sock = new ip::tcp::acceptor(*_ctx, ip::tcp::endpoint(ip::address::from_string("127.0.0.1"), 8888));
    _acceptor_holder.emplace_back([ac_sock] {
        acceptTCP(ac_sock, *_ctx);
    });
}

void AcceptorManager::initWithContext(asio::io_context &ctx) {
    delete _ctx;
    _ctx = &ctx;
    init();
}

void AcceptorManager::addAcceptable(void (*ac)()) {

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
    _default_builder = [](ConnHolder *holder) -> dial_core {
        return {proxy::Direct::Dialer::startStat(), nullptr};
    };
    // auto n = new NetAddress(ConnType::TCP, AddrType::IPv4, "127.0.0.1", 23333);
    // _default_builder = [n](ConnHolder *holder) -> dial_core {
    //     return {proxy::AES128::Dialer::startStat("hello this is a cipher", holder), n};
    // };
}

void DialerManager::addDialerBuilder(string tag, dial_core_builder builder) {
    if (tag.empty())
        return;
    _tagged_dialer[std::move(tag)] = std::move(builder);
}

dial_core DialerManager::getDialerForHolder(const string &tag, ConnHolder *holder) {
    if (_tagged_dialer.count(tag))
        return _tagged_dialer[tag](holder);
    return getDefaultDialerForHolder(holder);
}

dial_core DialerManager::getDefaultDialerForHolder(ConnHolder *holder) {
    return _default_builder(holder);
}
