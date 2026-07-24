#ifndef _KERNEL_TTY_H
#define _KERNEL_TTY_H 1

#include <stddef.h>
#include <stdint.h>
#include <sys/cdefs.h>

__attribute__((no_stack_protector)) void terminal_initialize(void);
void terminal_putchar(char c);
void terminal_write(const char *data, size_t size);
void terminal_writestring(const char *data);
void terminal_setcolor(uint8_t color);
void terminal_resetcolor();

#endif
