#include "proxy/aes_test.h"

#include "connection/conn_holder.h"
#include "connection/connection.h"

#include "cipher/cipher.h"

using namespace proxy::AES128;

Dialer::Dialer(cipher_p cipher) : _cipher(cipher) {}

void Dialer::stop() {
    delete this;
}

proxy::DialStrategy *Dialer::startStat(cipher_p cipher, ConnHolder *holder) {
    return new Handshake(std::move(cipher), holder);
}

DialHandshake::DialHandshake(cipher_p cipher, ConnHolder *holder) : Dialer(cipher) {}

ssize_t DialHandshake::toOutWrite(ConnHolder *holder, OutConn *out) {
    // read IV to in to out buffer
    holder->IOBufStream().write(
            (char *) _cipher->iv().data(),
            _cipher->iv().size()
    );
    cout << endl;

    try {
        _cipher->encryptTo(holder->IOBufStream(), holder->IOBufStream(), holder->IOSize() - AES::DEFAULT_KEYLENGTH);
    } catch (Exception &e) {
        cout << holder->id() << ": AES encryption error in toOutWrite: " << e.what() << endl;
        return -1;
    }
    holder->outWrite();
    return 0;
}

// this will never be called in state Handshake
ssize_t DialHandshake::onOutRead(ConnHolder *holder, OutConn *out) {
    try {
        _cipher->decryptTo(holder->OIBufStream(), holder->OIBufStream(), holder->OISize());
    } catch (Exception &e) {
        cout << holder->id() << ": AES decryption error in onOutRead: " << e.what() << endl;
        return -1;
    }
    holder->toInWrite();
    return 0;
}

ssize_t DialHandshake::onOutWrite(ConnHolder *holder, OutConn *out) {
    if (holder->IOBuf().size() != 0) {
        holder->outWrite();
    } else {
        out->strategy() = new Established(_cipher, holder);
        delete this;
        holder->toInRead();
    }
    return 0;
}

DialEstablished::DialEstablished(cipher_p cipher, ConnHolder *holder) : Dialer(cipher) {}

ssize_t DialEstablished::toOutWrite(ConnHolder *holder, OutConn *out) {
    try {
        _cipher->encryptTo(holder->IOBufStream(), holder->IOBufStream(), holder->IOSize());
    } catch (Exception &e) {
        cout << holder->id() << ": AES encryption error in toOutWrite: " << e.what() << endl;
        return -1;
    }
    holder->outWrite();
    return 0;
}

ssize_t DialEstablished::onOutRead(ConnHolder *holder, OutConn *out) {
    try {
        _cipher->decryptTo(holder->OIBufStream(), holder->OIBufStream(), holder->OISize());
    } catch (Exception &e) {
        cout << holder->id() << ": AES decryption error in onOutRead: " << e.what() << endl;
        return -1;
    }
    holder->toInWrite();
    return 0;
}

ssize_t DialEstablished::onOutWrite(ConnHolder *holder, OutConn *out) {
    holder->toInRead();
    return 0;
}
