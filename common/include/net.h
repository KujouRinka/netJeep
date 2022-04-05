#ifndef NETCAT_NET_H
#define NETCAT_NET_H

#include <string>

#include "types.h"

using namespace std;

class NetAddress {
public:
    NetAddress() = default;
    NetAddress(ConnType ct, AddrType at,
               const string &addr,
               uint16_t port)
            : _conn_type(ct),
              _addr_type(at),
              _remote_address(addr),
              _remote_port(port) {}

    string &addr() {
        return _remote_address;
    }

    uint16_t &port() {
        return _remote_port;
    }

    ConnType &conn_type() {
        return _conn_type;
    }

    AddrType &addr_type() {
        return _addr_type;
    }

    string &parsed_addr() {
        return _parsed_address;
    }

    void parse() {
        // TODO: do parse job
    }

private:
    string _remote_address;
    uint16_t _remote_port;
    ConnType _conn_type;
    AddrType _addr_type;
    string _parsed_address;
};

#endif