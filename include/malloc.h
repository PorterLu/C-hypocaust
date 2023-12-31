#ifndef __MALLOC_H__
#define __MALLOC_H__

#include "types.h"

#define MALLOC_ITEM_SIZE  512
#define TOTAL_ITEM_NUM 1024
#define MALLOC_CONTENT_SIZE (MALLOC_ITEM_SIZE - sizeof(uintptr_t)*2)

typedef struct malloc_item {
  void *item;
  uint8_t pad[MALLOC_ITEM_SIZE - sizeof(uintptr_t)*2];
  struct malloc_item *next;
} malloc_item;

void heap_init();
uint8_t* malloc();
bool free(void* addr);
void heap_test();

#endif