#include "malloc.h"
#include "vm.h"
#include "string.h"
#include "print.h"

// free list of malloc_item
static malloc_item *head;
static malloc_item *tail;

// applied list of malloc_item
static malloc_item *occupied_head;
static malloc_item *occupied_tail;

// apply 1024/8 pages for heap
void heap_init() {
	printf("heap init...\n");
  malloc_item *pre = NULL;
  for(int i = 0; i < TOTAL_ITEM_NUM / 8; i++) {
    tail = page_alloc(1);
    if(head != NULL) {
      pre->next = tail;
    } else {
      head = tail;
    }
    for(int j = 0; j < PGSIZE / MALLOC_ITEM_SIZE; j++) {
      tail->item = tail;
      tail->next = (uint8_t*)tail + 512;
      tail = tail->next;
    }
		pre =  (uint8_t*)tail - MALLOC_ITEM_SIZE;
  }
  tail = pre;
  tail->next = NULL;
}

// find an item in free list, add it into applied list
uint8_t* malloc() {
  if(head) {
    if(occupied_head == NULL) {
      occupied_head = occupied_tail = head;
      head = head->next;
      occupied_head->next = NULL;
    } else {
      occupied_tail->next = head;
      head = head->next;
      occupied_tail = occupied_tail->next;
      occupied_tail->next = NULL;
    }
    return occupied_tail;
  } 
  return NULL;
}

// free an item, add it into free list
bool free(void *addr) {
  if((uint64_t)addr % MALLOC_ITEM_SIZE != 0) {
    return false;
  }

  malloc_item *tmp = occupied_head;
  malloc_item *pre = NULL;
  while(tmp) {
    if(tmp == addr) {
      if(occupied_head == tmp) {
        tail->next = occupied_head;
        occupied_head = occupied_head->next;
        tail = tail->next;
        memset(tail, 0, MALLOC_ITEM_SIZE);
      } else {
        tail->next = tmp;
        pre->next = tmp->next;
        memset(tmp, 0, MALLOC_ITEM_SIZE);
        tail = tail->next;
      }
    }
    pre = tmp;
    tmp = tmp->next;
  }
}

void heap_test() {
	printf("heap test...\n");
  int count = 0;
  malloc_item *tmp = head;
  while(tmp != NULL) {
    count ++;
    printf("%lx\n", (uint64_t)(tmp->item));
    tmp = tmp->next;
  }
  printf("count:%d\n", count);
}