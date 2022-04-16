#ifndef NETJEEP_PROXY_SOCKS_H
#define NETJEEP_PROXY_SOCKS_H

#include "interface.h"

#include <cstdint>

#include <mutex>
#include <string>

#include "common/types.h"

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

        static AcceptStrategy *startStat();
    };

    class Negotiation : public Acceptor {
    public:
        ssize_t onInRead(ConnHolder *holder, InConn *in) override;
        ssize_t onInWrite(ConnHolder *holder, InConn *in) override;
        static AcceptStrategy *instance();

    private:
        Negotiation() = default;
        static AcceptStrategy *_self;
        static std::once_flag _of;
    };

    class ReqParse : public Acceptor {
    public:
        ssize_t onInRead(ConnHolder *holder, InConn *in) override;
        ssize_t onInWrite(ConnHolder *holder, InConn *in) override;
        static AcceptStrategy *instance();

    private:
        ReqParse() = default;
        static AcceptStrategy *_self;
        static std::once_flag _of;
    };

    class Established : public Acceptor {
    public:
        ssize_t onInRead(ConnHolder *holder, InConn *in) override;
        ssize_t onInWrite(ConnHolder *holder, InConn *in) override;
        static AcceptStrategy *instance();

    private:
        Established() = default;
        static AcceptStrategy *_self;
        static std::once_flag _of;
    };
}

#endif
