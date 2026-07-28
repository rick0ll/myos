

#ifndef _KERNEL_PANIC_H
#define _KERNEL_PANIC_H 1

#include <kernel/logger.h>
#include <kernel/tty.h>
#include <sys/cdefs.h>

__attribute__((__noreturn__)) void panic(const char *msg);

#endif
