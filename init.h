#ifndef NETJEEP_INIT_H
#define NETJEEP_INIT_H

#include "asio.hpp"

#include "config/config.h"
#include "proxy_manager/manager.h"

asio::io_context global_ctx;
const char *global_config_path;
Config::Config *global_config;

void init() {
    try {
        global_config = Config::loadConfig(global_config_path);
    } catch (const std::exception &e) {
        std::cerr << "config file format error" << std::endl;
        exit(1);
    }
    AcceptorManager::initWithContext(global_ctx);
    DialerManager::init();
}

static int f = []() {
    return 0;
}();

#endif
