#ifndef __PAGE_H__
#define __PAGE_H__

extern void *page_alloc(int npages);
extern void page_free(void *p);

#endif