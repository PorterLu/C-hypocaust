#include "string.h"

void *memset(void *s, int c, size_t n) {
  size_t i;
  if((int64_t)n < 0) return s;
  for(i=0; i<n; i++)
    *((char*)s + i) = c;
  return s;
}
