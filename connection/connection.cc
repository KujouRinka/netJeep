#include "connection.h"

#include <utility>

#include "conn_holder.h"

void InConn::toInRead(holder_p holder) {
  inRead(std::move(holder));
}

void InConn::toInWrite(holder_p holder) {
  inWrite(std::move(holder));
}

void OutConn::toOutRead(holder_p holder) {
  outRead(std::move(holder));
}

void OutConn::toOutWrite(holder_p holder) {
  outWrite(std::move(holder));
}
