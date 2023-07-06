#ifndef __STRING_H__
#define __STRING_H__
#include "types.h"

void *memset(void *s, int c, size_t n);
void *memcpy(void *out, const void *in, size_t n);
int strcmp(const char *s1, const char *s2);

#endif