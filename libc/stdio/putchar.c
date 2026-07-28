#include <stdint.h>
#include <stdio.h>

#if defined(__KERNEL__)
#include <kernel/tty.h>
#endif

int putchar(const unsigned char c) {
#if defined(__KERNEL__)
  terminal_putchar(c);
#else

// TODO: Implement stdio and the write system call.
#endif
  return c;
}
