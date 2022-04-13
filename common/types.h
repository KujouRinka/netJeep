#ifndef NETJEEP_TYPES_H
#define NETJEEP_TYPES_H

#include <memory>
#include <asio.hpp>

class ConnHolder;
class InConn;
class OutConn;

constexpr ssize_t MAX_BUF_SIZE = 2048;

using holder_p = std::shared_ptr<ConnHolder>;

using tcp_sock = asio::ip::tcp::socket;
using udp_sock = asio::ip::udp::socket;
using raw_sock = asio::ip::icmp::socket;

using tcp_sock_p = std::shared_ptr<tcp_sock>;
using udp_sock_p = std::shared_ptr<udp_sock>;
using raw_sock_p = std::shared_ptr<raw_sock>;

using in_p = std::shared_ptr<InConn>;
using out_p = std::shared_ptr<OutConn>;

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

enum class CloseType {
    both = 0,
    read,
    write,
};

#endif