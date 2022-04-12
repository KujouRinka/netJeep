#ifndef NETCAT_AES_TEST_H
#define NETCAT_AES_TEST_H

#include "proxy_interface.h"

#include <string>
#include <memory>

#include "cipher/cipher.h"

using namespace std;

namespace proxy::AES128 {
    using cipher_p = shared_ptr<cipher::AES128::Cipher>;
    class AcceptHandshake;
    class AcceptEstablished;
    class DialHandshake;
    class DialEstablished;

    class Acceptor : public AcceptStrategy {
    public:
        using Handshake = AcceptHandshake;
        using Established = AcceptEstablished;
    public:
        Acceptor(cipher_p cipher, ConnHolder *holder);
        virtual ~Acceptor() = default;

        void stop() override;

        static AcceptStrategy *startStat(cipher_p cipher, ConnHolder *holder);

    protected:
        uint8_t _pumped;
        cipher_p _cipher;
    };

    class AcceptHandshake : public Acceptor {
    public:
        AcceptHandshake(cipher_p cipher, ConnHolder *holder);
        ssize_t toInWrite(ConnHolder *holder, InConn *in) override;     // encrypt before write
        ssize_t onInRead(ConnHolder *holder, InConn *in) override;      // decrypt after read
        ssize_t onInWrite(ConnHolder *holder, InConn *in) override;
    };

    class AcceptEstablished : public Acceptor {
    public:
        AcceptEstablished(cipher_p cipher, ConnHolder *holder);
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

        static DialStrategy *startStat(cipher_p cipher, ConnHolder *holder);

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

}

#endif
