#include <iostream>
#include <asio.hpp>
#include <memory>

#include "Proxy/TCPConn.h"

#include <mutex>

using namespace std;
using namespace asio;

using tcp_sock = ip::tcp::socket;
using tcp_sock_p = shared_ptr<tcp_sock>;

static io_context ctx;
static ip::tcp::acceptor listen_sock(ctx, ip::tcp::endpoint(ip::address::from_string("127.0.0.1"), 8888));

long long conn_opened = 0;
mutex closed_count_lock;
long long conn_closed = 0;
long long conn_id = 0;

void to_listen() {
    tcp_sock_p sp(new tcp_sock(ctx));
    listen_sock.async_accept(*sp, [sp](const error_code err) {
        if (!err) {
            ++conn_opened;
            std::make_shared<TCPConn>(ctx, sp, conn_id++)->in_read();
        }
        to_listen();
    });
}

void runCtx() {
    ctx.run();
}

int main() {
    to_listen();
    signal_set signal_set(ctx);
    signal_set.add(SIGINT);
    signal_set.add(SIGTERM);
    signal_set.async_wait([&](const error_code error, int signum) {
        cout << "opened conn: " << conn_opened << endl
             << "closed conn: " << conn_closed << endl
             << "conn leaked: " << conn_opened - conn_closed << endl;
        exit(0);
    });
    runCtx();
    return 0;
}