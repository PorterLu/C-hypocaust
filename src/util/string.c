#include "string.h"
#include "malloc.h"
#include "page.h"

void *memset(void *s, int c, size_t n) {
  size_t i;
  if((int64_t)n < 0) return s;
  for(i=0; i<n; i++)
    *((char*)s + i) = c;
  return s;
}

void *memmove(void *dst, const void *src, size_t n) {
  char* tmp = page_alloc(PGROUNDUP(n)/PGSIZE);

  size_t i;

  //firstly, we copy the source to tmp array
  for(i = 0; i < n; i++)
    tmp[i] = *((char*) src + i);

  //then we move data to destination
  for(i = 0; i < n; i++)
      *((char*) dst + i) = tmp[i];

  page_free(tmp);
  return dst;
}

void *memcpy(void *out, const void *in, size_t n) {
  return memmove(out, in, n);
}

int strcmp(const char *s1, const char *s2) {
    size_t i = 0;
    while(s1[i] != '\0' && s2[i] != '\0')
    {
        if(s1[i] == s2[i])
            i++;
        else
            return s1[i] - s2[i];
    }

    return s1[i] - s2[i];
}
