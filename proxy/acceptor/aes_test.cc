#include "proxy/aes_test.h"

#include <utility>

#include "connection/connection.h"
#include "connection/conn_holder.h"

using namespace proxy::AES128;

Acceptor::Acceptor(cipher_p cipher) : _cipher(std::move(cipher)) {}

void Acceptor::stop() {
    delete this;
}

proxy::AcceptStrategy *Acceptor::startStat(cipher_p cipher) {
    return new Handshake(std::move(cipher));
}

AcceptHandshake::AcceptHandshake(cipher_p cipher) : Acceptor(std::move(cipher)) {}

// Do nothing. This won't be called in this stage
ssize_t AcceptHandshake::toInWrite(ConnHolder *holder, InConn *in) {
    return 0;
}

ssize_t AcceptHandshake::onInRead(ConnHolder *holder, InConn *in) {
    // parse shadowsocks header: [iv][ss header][payload]
    FileSource fs(holder->IOBufStream(), false, new ArraySink(_cipher->iv(), 16));
    if (fs.Pump(16) != 16) {
        cout << holder->id() << ": iv parsed error" << endl;
        holder->closeAll();
    }
    _cipher->flushCipher();
    try {
        _cipher->decryptTo(holder->IOBufStream(), holder->IOBufStream(), holder->IOSize());
    } catch (Exception &e) {
        cout << holder->id() << ": shadowsocks header decryption error" << endl;
        holder->closeAll();
    }

    // parse: [ss header][payload]
    uint8_t type;
    holder->IOBufStream().read((char *) &type, 1);
    uint8_t next_min_len;
    if (type == 0x01) {         // IPv4
        next_min_len = 8;
    } else if (type == 0x03) {  // DOMAIN
        if (holder->IOSize() < 1)
            return -1;
        holder->IOBufStream().read((char *) &next_min_len, 1);
        next_min_len += 2;
    } else if (type == 0x04) {  // IPv6
        next_min_len = 18;
        cout << holder->id() << ": unsupported type" << endl;
    } else {
        cout << holder->id() << ": unsupported type" << endl;
    }

    if (holder->IOSize() < next_min_len)
        return -1;

    uint8_t buffer[257];    // address & port
    holder->IOBufStream().read((char *) buffer, next_min_len);
    string address;
    uint16_t port = ((uint16_t) *(buffer + next_min_len - 2) << 8) + *(buffer + next_min_len - 1);
    if (type == 0x01) {     // parse IPv4
        holder->remote().addr_type() = AddrType::IPv4;
        address.append(to_string(buffer[0]))
                .append(1, '.')
                .append(to_string(buffer[1]))
                .append(1, '.')
                .append(to_string(buffer[2]))
                .append(1, '.')
                .append(to_string(buffer[3]));
    } else if (type == 0x03) {  // parse DOMAIN
        holder->remote().addr_type() = AddrType::Domain;
        address.append(reinterpret_cast<const char *>(buffer), next_min_len - 2);
    } else if (type == 0x04) {  // parse IPv6
        holder->remote().addr_type() = AddrType::IPv6;
        // TODO: parse IPv6, temporary unsupported
        cout << holder->id() << ": unsupported address type" << endl;
        return -1;
    }
    holder->remote().addr() = std::move(address);
    holder->remote().port() = port;
    holder->remote().conn_type() = ConnType::TCP;

    // parse payload: [payload]
    in->strategy() = new Established(_cipher);
    delete this;
    holder->dial();
    return 0;
}

ssize_t AcceptHandshake::onInWrite(ConnHolder *holder, InConn *in) {
    return 0;
}

AcceptEstablished::AcceptEstablished(cipher_p cipher) : Acceptor(std::move(cipher)) {}

ssize_t AcceptEstablished::toInWrite(ConnHolder *holder, InConn *in) {
    try {
        _cipher->encryptTo(holder->OIBufStream(), holder->OIBufStream(), holder->OISize());
    } catch (Exception &e) {
        cout << holder->id() << ": cipher encryption error in toInWrite: " << e.what() << endl;
        return -1;
    }
    holder->inWrite();
    return 0;
}

ssize_t AcceptEstablished::onInRead(ConnHolder *holder, InConn *in) {
    try {
        _cipher->decryptTo(holder->IOBufStream(), holder->IOBufStream(), holder->IOSize());
    }
    catch (Exception &e) {
        cout << holder->id() << ": cipher decryption error in onInRead: " << e.what() << endl;
        return -1;
    }
    holder->toOutWrite();
    return 0;
}

ssize_t AcceptEstablished::onInWrite(ConnHolder *holder, InConn *in) {
    holder->toOutRead();
    return 0;
}
