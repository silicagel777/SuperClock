#include "util/new.h"

void *operator new(size_t size, void *ptr) {
  return ptr;
}
