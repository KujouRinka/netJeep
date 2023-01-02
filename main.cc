#include <iostream>

#include "asio.hpp"

#include "proxy_manager/manager.h"

#include "init.h"

using namespace std;
using namespace asio;

int main(int argc, char **argv) {
  cout << "config path: " << argv[1] << endl;
  global_config_path = argv[1];
  init();
  signal_set signal_set(global_ctx);
  signal_set.add(SIGINT);
  signal_set.add(SIGTERM);
  signal_set.async_wait([&](const error_code error, int signum) {
    exit(0);
  });
  AcceptorManager::acceptAll();   // block!
  return 0;
}