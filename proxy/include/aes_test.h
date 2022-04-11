#ifndef NETCAT_AES_TEST_H
#define NETCAT_AES_TEST_H

#include "proxy_interface.h"

#include <string>

#include <cryptopp/cryptlib.h>
#include <cryptopp/rijndael.h>
#include <cryptopp/sha.h>
#include <cryptopp/modes.h>
#include <cryptopp/osrng.h>
#include <cryptopp/files.h>

using namespace std;
using namespace CryptoPP;

namespace proxy::AES128 {

    class Cipher {
    public:
        Cipher(const char *raw_key)
                : _iv(CryptoPP::AES::BLOCKSIZE) {
            SHA256 hash;
            hash.Update((CryptoPP::byte *) raw_key, strlen(raw_key));
            string digest;
            digest.resize(CryptoPP::AES::DEFAULT_KEYLENGTH);
            hash.TruncatedFinal((CryptoPP::byte *) digest.data(), CryptoPP::AES::DEFAULT_KEYLENGTH);
            _key.Append((CryptoPP::byte *) digest.data(), CryptoPP::AES::DEFAULT_KEYLENGTH);
            AutoSeededRandomPool prng;
            prng.GenerateBlock(_iv, _iv.size());
            _encryption.SetKeyWithIV(_key, _key.size(), _iv);
            _decryption.SetKeyWithIV(_key, _key.size(), _iv);
        }

        void encryptTo(istream &is, ostream &os, size_t sz) {
            FileSource fs(is, false, new StreamTransformationFilter(_encryption, new FileSink(os)));
            fs.Pump(sz);
        }

        void decryptTo(istream &is, ostream &os, size_t sz) {
            FileSource fs(is, true, new StreamTransformationFilter(_decryption, new FileSink(os)));
            fs.Pump(sz);
        }

        auto &iv() {
            return _iv;
        }

    private:
        SecByteBlock _iv;
        SecByteBlock _key;
        CFB_Mode<CryptoPP::AES>::Encryption _encryption;
        CFB_Mode<CryptoPP::AES>::Decryption _decryption;
    };

    class Acceptor : public AcceptStrategy {
    public:
        Acceptor(Cipher *cipher, ConnHolder *holder);
        ssize_t toInWrite(ConnHolder *holder, InConn *in) override;     // encrypt before write
        ssize_t onInRead(ConnHolder *holder, InConn *in) override;      // decrypt after read
        ssize_t onInWrite(ConnHolder *holder, InConn *in) override;

        void stop() override;

    private:
        uint8_t _pumped;
        proxy::AES128::Cipher *_cipher;
    };

    class Dialer : public DialStrategy {
    public:
        Dialer(Cipher *cipher, ConnHolder *holder);
        ssize_t toOutWrite(ConnHolder *holder, OutConn *out) override;  // encrypt before write
        ssize_t onOutRead(ConnHolder *holder, OutConn *out) override;   // decrypt after read
        ssize_t onOutWrite(ConnHolder *holder, OutConn *out) override;

        void stop() override;

    private:
        proxy::AES128::Cipher *_cipher;
    };

    class Handshake : public Dialer {

    };

    class Established : public Dialer {

    };

}

#endif
