#ifndef NETCAT_BUFFER_H
#define NETCAT_BUFFER_H

// #include "types.h"
#include <cstdlib>
#include <cstring>
#include <cstdint>

// TODO: implement high robust buffer
static constexpr ssize_t BUF_CAP = 2048;
class Buffer {
public:
    Buffer() : _len(0), _cap(BUF_CAP) {};
    uint8_t *get() {
        return _buf;
    }

    ssize_t rewrite(const char *data, ssize_t size) {
        if (size > _cap)
            return -1;
        memcpy(_buf, data, size);
        _len = size;
        return size;
    }

    ssize_t size() {
        return _len;
    }

    ssize_t resize(ssize_t size) {
        if (size > BUF_CAP)
            return -1;
        _len = size;
        return size;
    }

private:
    uint8_t _buf[BUF_CAP];
    ssize_t _len;
    ssize_t _cap;
};

#endif