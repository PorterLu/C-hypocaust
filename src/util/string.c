#include "string.h"

static buffer[512];

void *memset(void *s, int c, size_t n) {
  size_t i;
  if((int64_t)n < 0) return s;
  for(i=0; i<n; i++)
    *((char*)s + i) = c;
  return s;
}

void *memmove(void *dst, const void *src, size_t n) {
  //char* tmp = (char *)malloc(n);
  char* tmp = buffer;
  size_t i;

  //firstly, we copy the source to tmp array
  for(i = 0; i < n; i++)
    tmp[i] = *((char*) src + i);

  //then we move data to destination
  for(i = 0; i < n; i++)
      *((char*) dst + i) = tmp[i];

  //free(tmp);
  return dst;
}

void *memcpy(void *out, const void *in, size_t n) {
  return memmove(out, in, n);
}