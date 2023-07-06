#ifndef __PAGE_H__
#define __PAGE_H__

extern void *page_alloc(int npages);
extern void page_free(void *p);
#define PGSIZE 4096 // bytes per page
#define PGSHIFT 12  // bits of offset within a page
#define PGROUNDUP(sz)  (((sz)+PGSIZE-1) & ~(PGSIZE-1))
#define PGROUNDDOWN(a) (((a)) & ~(PGSIZE-1))

#endif