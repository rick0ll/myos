#if defined __KERNEL__
#include <kernel/panic.h>

#else
#include <stdio.h>

#endif

#include <stdlib.h>

__attribute__((__noreturn__)) void abort(void) {
#if defined __KERNEL__
  panic("abort()");
#else
  // TODO: Abnormally terminate the process as if by SIGABRT.
  printf("abort()\n");
  while (1) {
  }
#endif
  __builtin_unreachable();
}
