#ifndef NETCAT_TYPES_H
#define NETCAT_TYPES_H

#include <memory>
#include <asio.hpp>

class RealConn;

using conn_p = std::shared_ptr<RealConn>;

using tcp_sock = asio::ip::tcp::socket;
using udp_sock = asio::ip::udp::socket;
using raw_sock = asio::ip::icmp::socket;

using tcp_sock_p = std::shared_ptr<tcp_sock>;
using udp_sock_p = std::shared_ptr<udp_sock>;
using raw_sock_p = std::shared_ptr<raw_sock>;

union out_sock_union {
    tcp_sock_p tcp;
    udp_sock udp;
};

using out_sock_p = std::shared_ptr<out_sock_union>;

enum class ConnType {
    RAW = 0,
    TCP,
    UDP,
};

enum class AddrType {
    IPv4 = 0,
    IPv6,
    Domain,
};

#endif