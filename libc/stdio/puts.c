#include <stdio.h>

int puts(char *__restrict__ string) {
  return printf("{s}\n", (const unsigned char *)string);
}
