#ifndef NETJEEP_PROXY_DIRECT_H
#define NETJEEP_PROXY_DIRECT_H

#include "interface.h"

#include "proxy_manager/types.h"

#include <string>

namespace Config {
    class Dialer;
}

namespace proxy::Direct {

    /**
     * @brief DialDirect implements how data transferred directly.
     */
    class Dialer : public DialStrategy {
    public:
        ssize_t onOutRead(ConnHolder *holder, OutConn *out) override;
        ssize_t onOutWrite(ConnHolder *holder, OutConn *out) override;
        static void init();
        static DialStrategy *startStat();

    private:
        static DialStrategy *_self;
    };

    dialCoreBuilder dialBuilderFromConfig(Config::Dialer &d);
}

#endif