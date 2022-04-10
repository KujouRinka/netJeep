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

class AESCrypto {
public:
    AESCrypto(const char *raw_key)
            : _iv(AES::BLOCKSIZE) {
        SHA256 hash;
        hash.Update((byte *) raw_key, strlen(raw_key));
        string digest;
        digest.resize(AES::DEFAULT_KEYLENGTH);
        hash.TruncatedFinal((byte *) digest.data(), AES::DEFAULT_KEYLENGTH);
        _key.Append((byte *) digest.data(), AES::DEFAULT_KEYLENGTH);
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

    auto iv() {
        return _iv;
    }

private:
    SecByteBlock _iv;
    SecByteBlock _key;
    CFB_Mode<AES>::Encryption _encryption;
    CFB_Mode<AES>::Decryption _decryption;
};

class AcceptAES : public AcceptStrategy {
public:
    AcceptAES(AESCrypto *cipher);
    ssize_t toInWrite(ConnHolder *holder, InConn *in) override;     // encrypt before write
    ssize_t onInRead(ConnHolder *holder, InConn *in) override;      // decrypt after read
    ssize_t onInWrite(ConnHolder *holder, InConn *in) override;

public:
    AESCrypto *_cipher;
};

class DialAES : public DialStrategy {
public:
    DialAES(AESCrypto *cipher);
    ssize_t toOutWrite(ConnHolder *holder, OutConn *out) override;  // encrypt before write
    ssize_t onOutRead(ConnHolder *holder, OutConn *out) override;   // decrypt after read
    ssize_t onOutWrite(ConnHolder *holder, OutConn *out) override;

private:
    AESCrypto *_cipher;
};

#endif