#ifndef __HYPERVISOR_H__
#define __HYPERVISOR_H__

#include "types.h"
#include "platform.h"

#include <stdarg.h>

extern int uart_putc(char ch);
extern void uart_puts(char *s);

extern int printf(const char *s, ...);
extern void panic(char *s);

extern void *page_alloc(int npages);
extern void page_free(void *p);

void init_hypervisor();

#define concat(a, b) a ## b

#endif
