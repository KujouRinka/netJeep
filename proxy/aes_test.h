#ifndef NETJEEP_PROXY_AES_TEST_H
#define NETJEEP_PROXY_AES_TEST_H

#include "interface.h"

#include "asio.hpp"

#include <string>
#include <memory>

#include "proxy_manager/types.h"
#include "cipher/cipher.h"

using namespace std;

namespace Config {
    struct Acceptor;
    struct Dialer;
}

namespace proxy::AES128 {
    using cipher_p = shared_ptr<cipher::AES::Cipher<128>>;
    class AcceptHandshake;
    class AcceptEstablished;
    class DialHandshake;
    class DialEstablished;

    class Acceptor : public AcceptStrategy {
    public:
        using Handshake = AcceptHandshake;
        using Established = AcceptEstablished;
    public:
        explicit Acceptor(cipher_p cipher);
        virtual ~Acceptor() = default;

        void stop() override;

        static AcceptStrategy *startStat(cipher_p cipher);

    protected:
        cipher_p _cipher;
    };

    class AcceptHandshake : public Acceptor {
    public:
        explicit AcceptHandshake(cipher_p cipher);
        ssize_t toInWrite(ConnHolder *holder, InConn *in) override;     // encrypt before write
        ssize_t onInRead(ConnHolder *holder, InConn *in) override;      // decrypt after read
        ssize_t onInWrite(ConnHolder *holder, InConn *in) override;
    };

    class AcceptEstablished : public Acceptor {
    public:
        explicit AcceptEstablished(cipher_p cipher);
        ssize_t toInWrite(ConnHolder *holder, InConn *in) override;     // encrypt before write
        ssize_t onInRead(ConnHolder *holder, InConn *in) override;      // decrypt after read
        ssize_t onInWrite(ConnHolder *holder, InConn *in) override;
    };


    class Dialer : public DialStrategy {
    public:
        using Handshake = DialHandshake;
        using Established = DialEstablished;
    public:
        explicit Dialer(cipher_p cipher);
        virtual ~Dialer() = default;

        void stop() override;

        static DialStrategy *startStat(const char *passwd, ConnHolder *holder);

    protected:
        cipher_p _cipher;
    };


    class DialHandshake : public Dialer {
    public:
        DialHandshake(cipher_p cipher, ConnHolder *holder);
        ssize_t toOutWrite(ConnHolder *holder, OutConn *out) override;  // encrypt before write
        ssize_t onOutRead(ConnHolder *holder, OutConn *out) override;   // decrypt after read
        ssize_t onOutWrite(ConnHolder *holder, OutConn *out) override;
    };

    class DialEstablished : public Dialer {
    public:
        DialEstablished(cipher_p cipher, ConnHolder *holder);
        ssize_t toOutWrite(ConnHolder *holder, OutConn *out) override;  // encrypt before write
        ssize_t onOutRead(ConnHolder *holder, OutConn *out) override;   // decrypt after read
        ssize_t onOutWrite(ConnHolder *holder, OutConn *out) override;
    };

    void acceptTCP(asio::ip::tcp::acceptor *ac, asio::io_context &ctx, const char *passwd);
    acceptFunc acceptFuncFromConfig(asio::io_context &ctx, Config::Acceptor &a);
    dialCoreBuilder dialBuilderFromConfig(Config::Dialer &d);
}

#endif
