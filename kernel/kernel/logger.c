#include <kernel/logger.h>
#include <kernel/tty.h>
#include <kernel/vga.h>

#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>

int kernel_log_internal(uint8_t color, const char *__restrict__ level,
                        const char *__restrict__ str, ...) {
  terminal_setcolor(color);
  terminal_writestring(level);
  terminal_resetcolor();

  va_list args;
  va_start(args, str);

  int written = vprintf(str, args);

  va_end(args);

  return written;
}
