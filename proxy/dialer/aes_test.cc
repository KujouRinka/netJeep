#include "proxy/aes_test.h"

#include <utility>
#include <charconv>

#include "connection/conn_holder.h"
#include "connection/connection.h"

#include "cipher/cipher.h"

using namespace proxy::AES128;

Dialer::Dialer(cipher_p cipher) : _cipher(std::move(cipher)) {}

void Dialer::stop() {
    delete this;
}

proxy::DialStrategy *Dialer::startStat(const char *passwd, ConnHolder *holder) {
    cipher_p c(new cipher::AES::Cipher<128>(passwd));
    return new Handshake(std::move(c), holder);
}

DialHandshake::DialHandshake(cipher_p cipher, ConnHolder *holder) : Dialer(std::move(cipher)) {
    // prepare shadowsocks header
    uint8_t ss_header[259];
    uint16_t ss_header_len;
    auto &remote = holder->remote();
    auto type = remote.addr_type();
    if (type == AddrType::IPv4) {
        ss_header[0] = 0x01;
        ss_header_len = 7;
        auto &ip = remote.addr();
        auto last = ip.data() + ip.size();
        uint8_t *p = ss_header + 1;
        from_chars(ip.data(), last, *p);
        int cur = -1;
        for (int i = 1; i < 4; ++i) {
            cout << '.';
            cur = ip.find('.', cur + 1) + 1;
            from_chars(ip.data() + cur, last, *++p);
        }
    } else if (type == AddrType::Domain) {
        ss_header[0] = 0x03;
        ss_header[1] = remote.addr().size();
        memcpy(ss_header + 2, remote.addr().data(), ss_header[1]);
        ss_header_len = ss_header[1] + 4;
    } else if (type == AddrType::IPv6) {
        ss_header[0] = 0x04;
        ss_header_len = 19;
        cout << holder->id() << ": unsupported address type for shadowsocks" << endl;
    } else {
        cout << holder->id() << ": unsupported address type for shadowsocks" << endl;
    }
    ss_header[ss_header_len - 2] = remote.port() >> 8;
    ss_header[ss_header_len - 1] = remote.port();

    // write IV and shadowsocks header to in to out buffer
    holder->IOBufStream().write(
            (char *) _cipher->iv().data(),
            _cipher->iv().size()
    );
    holder->IOBufStream().write((char *) ss_header, ss_header_len);
}

ssize_t DialHandshake::toOutWrite(ConnHolder *holder, OutConn *out) {
    // current IOBufStream: [iv][ss header][payload]
    // move iv to buffer tail
    char iv_buf[16];
    holder->IOBufStream().read(iv_buf, 16);
    holder->IOBufStream().write(iv_buf, 16);

    // encrypt ss header and payload
    try {
        _cipher->encryptTo(holder->IOBufStream(), holder->IOBufStream(), holder->IOSize() - AES::DEFAULT_KEYLENGTH);
    } catch (Exception &e) {
        cout << holder->id() << ": AES encryption error in toOutWrite: " << e.what() << endl;
        return -1;
    }
    // current IOBufStream: [iv][ss header(encrypted)][payload(encrypted)]
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

DialEstablished::DialEstablished(cipher_p cipher, ConnHolder *holder) : Dialer(std::move(cipher)) {}

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
