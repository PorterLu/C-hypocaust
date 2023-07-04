#ifndef __PRINT_H__
#define __PRINT_H__

extern int uart_putc(char ch);
extern void uart_puts(char *s);

extern int printf(const char *s, ...);
extern void panic(char *s);
extern void unreachable();

#endif