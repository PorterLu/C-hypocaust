#ifndef __MALLOC_H__
#define __MALLOC_H__

#include "types.h"

#define MALLOC_ITEM_SIZE  512
#define TOTAL_ITEM_NUM 1024

typedef struct malloc_item {
  void *item;
  uint8_t pad[MALLOC_ITEM_SIZE - sizeof(uintptr_t)*2];
  struct malloc_item *next;
} malloc_item;

void heap_init();
bool malloc();
bool free(void* addr);
void heap_test();

#endif