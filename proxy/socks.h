#ifndef NETJEEP_PROXY_SOCKS_H
#define NETJEEP_PROXY_SOCKS_H

#include "interface.h"

#include <cstdint>

#include <string>

#include "common/types.h"
#include "proxy_manager/types.h"

namespace Config {
struct Acceptor;
}

namespace proxy::Socks {
/**
 * @brief AcceptSocks implements socks handshake of passive endpoints.
 *
 * @details https://en.wikipedia.org/wiki/SOCKS
 *
 */
class Acceptor : public AcceptStrategy {
 public:
  ssize_t onInRead(ConnHolder *holder, InConn *in) override = 0;
  ssize_t onInWrite(ConnHolder *holder, InConn *in) override = 0;

  static void init();
  static AcceptStrategy *startStat();
};

class Negotiation : public Acceptor {
 public:
  ssize_t onInRead(ConnHolder *holder, InConn *in) override;
  ssize_t onInWrite(ConnHolder *holder, InConn *in) override;
  static void init();
  static AcceptStrategy *instance();

 private:
  Negotiation() = default;
  static AcceptStrategy *_self;
};

class ReqParse : public Acceptor {
 public:
  ssize_t onInRead(ConnHolder *holder, InConn *in) override;
  ssize_t onInWrite(ConnHolder *holder, InConn *in) override;
  static void init();
  static AcceptStrategy *instance();

 private:
  ReqParse() = default;
  static AcceptStrategy *_self;
};

class Established : public Acceptor {
 public:
  ssize_t onInRead(ConnHolder *holder, InConn *in) override;
  ssize_t onInWrite(ConnHolder *holder, InConn *in) override;
  static void init();
  static AcceptStrategy *instance();

 private:
  Established() = default;
  static AcceptStrategy *_self;
};

void acceptTCP(asio::ip::tcp::acceptor *ac, asio::io_context &ctx);
acceptFunc acceptFuncFromConfig(asio::io_context &ctx, Config::Acceptor &a);
}

#endif
