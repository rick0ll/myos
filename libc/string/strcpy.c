

#include <string.h>

char *strcpy(unsigned char *__restrict__ dst,
             unsigned const char *__restrict__ text) {
  return memcpy(dst, text, strlen(text) + 1);
}
