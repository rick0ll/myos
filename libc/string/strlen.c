#include <string.h>

size_t strlen(const char *str) {
  const unsigned char *u_str = (const unsigned char *)str;
  size_t len = 0;
  while (u_str[len])
    len++;
  return len;
}
