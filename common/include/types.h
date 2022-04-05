#ifndef NETCAT_TYPES_H
#define NETCAT_TYPES_H

#include <memory>
#include <asio.hpp>

class BaseConn;
class InConn;
class OutConn;

using conn_p = std::shared_ptr<BaseConn>;

using tcp_sock = asio::ip::tcp::socket;
using udp_sock = asio::ip::udp::socket;
using raw_sock = asio::ip::icmp::socket;

using tcp_sock_p = std::shared_ptr<tcp_sock>;
using udp_sock_p = std::shared_ptr<udp_sock>;
using raw_sock_p = std::shared_ptr<raw_sock>;

using in_p = std::shared_ptr<InConn>;
using out_p = std::shared_ptr<OutConn>;

// constexpr size_t BUF_CAP = 2048;
// using Buffer = uint8_t;

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