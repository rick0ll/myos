#include <stdint.h>
#include <string.h>

int strcmp(char *a, char *b) {
  uint32_t len_a = strlen(a);
  uint32_t len_b = strlen(b);

  uint32_t len = len_a < len_b ? len_a : len_b;
  return memcmp(a, b, len);
}
