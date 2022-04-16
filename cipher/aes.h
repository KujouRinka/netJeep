#ifndef NETJEEP_CIPHER_AES_H
#define NETJEEP_CIPHER_AES_H

#include <string>
#include <memory>

#include "cryptopp/cryptlib.h"
#include "cryptopp/rijndael.h"
#include "cryptopp/sha.h"
#include "cryptopp/modes.h"
#include "cryptopp/osrng.h"
#include "cryptopp/files.h"

using namespace std;
using namespace CryptoPP;

namespace cipher::AES {

    template<unsigned int KEY_LENGTH>
    class Cipher {
    public:

        // this constructor for dialer
        explicit Cipher(const char *raw_key)
                : _iv(CryptoPP::AES::BLOCKSIZE) {
            makeKey(raw_key);
            // generate IV
            AutoSeededRandomPool prng;
            prng.GenerateBlock(_iv, _iv.size());
            flushCipher();
        }

        // this constructor for listener
        Cipher(const char *raw_key, const char *iv)
                : _iv(CryptoPP::AES::BLOCKSIZE) {

        }

        void encryptTo(istream &is, ostream &os, size_t sz) {
            FileSource fs(is, false, new StreamTransformationFilter(_encryption, new FileSink(os)));
            fs.Pump(sz);
        }

        void decryptTo(istream &is, ostream &os, size_t sz) {
            FileSource fs(is, false, new StreamTransformationFilter(_decryption, new FileSink(os)));
            fs.Pump(sz);
        }

        auto &iv() {
            return _iv;
        }

        void flushCipher() {
            _encryption.SetKeyWithIV(_key, _key.size(), _iv);
            _decryption.SetKeyWithIV(_key, _key.size(), _iv);
        }

    private:
        SecByteBlock _iv;
        SecByteBlock _key;
        CFB_Mode<CryptoPP::AES>::Encryption _encryption;
        CFB_Mode<CryptoPP::AES>::Decryption _decryption;

        void makeKey(const char *raw_key) {
            SHA256 hash;
            hash.Update((CryptoPP::byte *) raw_key, strlen(raw_key));
            string digest;
            digest.resize(KEY_LENGTH / 8);
            hash.TruncatedFinal((CryptoPP::byte *) digest.data(), KEY_LENGTH / 8);
            _key.Append((CryptoPP::byte *) digest.data(), KEY_LENGTH / 8);
        }
    };

}

#endif