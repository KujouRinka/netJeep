#include "connection.h"

#include "conn_holder.h"

void InConn::toInRead(holder_p holder) {
    inRead(holder);
}

void InConn::toInWrite(holder_p holder) {
    inWrite(holder);
}

void OutConn::toOutRead(holder_p holder) {
    outRead(holder);
}

void OutConn::toOutWrite(holder_p holder) {
    outWrite(holder);
}
