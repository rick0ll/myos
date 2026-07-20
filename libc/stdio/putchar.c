#include <stdio.h>

#if defined(__KERNEL__)
#include <kernel/tty.h>
#endif

int putchar(int ic) {
#if defined(__KERNEL__)
  char c = (char)ic;
  terminal_putchar(c);
#else
  // TODO: Implement stdio and the write system call.
#endif
  return ic;
}
