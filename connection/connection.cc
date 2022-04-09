#include "connection.h"

ConnHolder *&InConn::holder() {
    return _holder;
}

AcceptStrategy *&InConn::strategy() {
    return _strategy;
}

ConnHolder *&OutConn::holder() {
    return _holder;
}

DialStrategy *&OutConn::strategy() {
    return _strategy;
}
