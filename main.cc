#include <iostream>
#include <asio.hpp>

#include "connection/tcp.h"
#include "connection/conn_holder.h"

#include "proxy/socks.h"
#include "proxy_manager/manager.h"

#include <mutex>

using namespace std;
using namespace asio;

io_context global_ctx;

long long conn_opened = 0;
mutex closed_count_lock;
long long conn_closed = 0;

void init() {
    AcceptorManager::initWithContext(global_ctx);
    DialerManager::init();
}

int main() {
    init();
    signal_set signal_set(global_ctx);
    signal_set.add(SIGINT);
    signal_set.add(SIGTERM);
    signal_set.async_wait([&](const error_code error, int signum) {
        cout << "opened conn: " << conn_opened << endl
             << "closed conn: " << conn_closed << endl
             << "conn leaked: " << conn_opened - conn_closed << endl;
        exit(0);
    });
    AcceptorManager::acceptAll();   // block!
    return 0;
}