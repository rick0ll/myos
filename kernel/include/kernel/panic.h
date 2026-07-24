

#ifndef _KERNEL_PANIC_H
#define _KERNEL_PANIC_H 1

#include <sys/cdefs.h>

__attribute__((__noreturn__)) void panic(const char *msg);

#endif
