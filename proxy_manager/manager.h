#ifndef NETJEEP_PROXY_MANAGER_H
#define NETJEEP_PROXY_MANAGER_H

#include <vector>
#include <unordered_map>
#include <utility>

#include "types.h"

using namespace std;

namespace asio {
class io_context;
}

class AcceptorManager {
 public:
  AcceptorManager() = delete;
  static void init();
  static void initWithContext(asio::io_context &ctx);

  static void addAcceptable(const string &tag, acceptFunc ac);
  static AcceptorManager *getAcceptor();
  static void acceptAll();

 private:
  static asio::io_context *_ctx;
  static vector<acceptFunc> _acceptor_holder;
};

class DialerManager {
 public:
  DialerManager() = delete;
  static void init();

  static void addDialerBuilder(string tag, dialCoreBuilder builder);
  static dialCore getDialCoreForHolder(const string &tag, ConnHolder *holder);
  static dialCore getDefaultDialerForHolder(ConnHolder *holder);

 private:
  static unordered_map<string, dialCoreBuilder> _tagged_dialer;
  static dialCoreBuilder _default_builder;
};

#endif
