
#include <stdarg.h>
#include <stdio.h>

int printf(const unsigned char *__restrict__ str, ...) {
  va_list parameters;
  va_start(parameters, str);

  int written = vprintf(str, parameters, false);

  va_end(parameters);
  return written;
}
