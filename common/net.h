#ifndef NETJEEP_NET_H
#define NETJEEP_NET_H

#include <string>

#include "types.h"

using namespace std;

/**
 * @brief NetAddress stores information of real remote target.
 */
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

    NetAddress(const NetAddress &rhs) {
        _remote_address = rhs._remote_address;
        _remote_port = rhs._remote_port;
        _conn_type = rhs._conn_type;
        _addr_type = rhs._addr_type;
        _parsed_address = rhs._parsed_address;
    }

    NetAddress &operator=(const NetAddress &rhs) {
        if (&rhs != this) {
            _remote_address = rhs._remote_address;
            _remote_port = rhs._remote_port;
            _conn_type = rhs._conn_type;
            _addr_type = rhs._addr_type;
            _parsed_address = rhs._parsed_address;
        }
        return *this;
    }

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