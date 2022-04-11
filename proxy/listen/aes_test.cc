#include "proxy/aes_test.h"

#include "connection/conn_holder.h"

using namespace proxy::AES128;

Acceptor::Acceptor(cipher_p cipher, ConnHolder *holder)
        : _pumped(0), _cipher(cipher) {

}

ssize_t Acceptor::toInWrite(ConnHolder *holder, InConn *in) {
    try {
        _cipher->encryptTo(holder->OIBufStream(), holder->OIBufStream(), holder->OISize());
    } catch (Exception &e) {
        cout << holder->id() << ": cipher encryption error in toInWrite: " << e.what() << endl;
        return -1;
    }
    holder->inWrite();
    return 0;
}

ssize_t Acceptor::onInRead(ConnHolder *holder, InConn *in) {
    try {
        if (_pumped < 16) {     // parse IV
            FileSource fs(holder->IOBufStream(), false,
                          new ArraySink(_cipher->iv().data() + _pumped, 16 - _pumped));
            _pumped += fs.Pump(16 - _pumped);
        }

        if (_pumped == 16)      // parse payload
            _cipher->decryptTo(holder->IOBufStream(), holder->IOBufStream(), holder->IOSize());

    }
    catch (Exception &e) {
        cout << holder->id() << ": cipher decryption error in onInRead: " << e.what() << endl;
        return -1;
    }
    holder->toOutWrite();
    return 0;
}

ssize_t Acceptor::onInWrite(ConnHolder *holder, InConn *in) {
    holder->toOutRead();
    return 0;
}

void Acceptor::stop() {
    delete this;
}

proxy::AcceptStrategy *Acceptor::startStat(cipher_p cipher, ConnHolder *holder) {
    return new ListenHandshake(std::move(cipher), holder);
}
