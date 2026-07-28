#include <limits.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int printText(const unsigned char *str, size_t len) {
  if (len > INT_MAX)
    return -1;

  const unsigned char *buff = str;

  for (size_t i = 0; i < len; i++) {
    if (putchar(buff[i]) == EOF)
      return -1;
  }

  return len;
}

int printVariable(const unsigned char *__restrict__ str, va_list *parameters) {
  int i = 0;
  char type = str[i + 1];

  switch (type) {
  case 's': {
    const char *var = va_arg(*parameters, const char *);
    size_t len = strlen(var);
    if (printText(var, len) == -1) {
      return -1;
    }
    return len;
  };

  case 'c': {
    char var = (char)va_arg(*parameters, int);
    if (printText(&var, 1) == -1) {
      return -1;
    }
    return 1;
  };
  case 'd': {
    int var = va_arg(*parameters, int);
    char num[11];
    int len = itoa(var, num);
    if (printText(num, len) == -1) {
      return -1;
    }
    return len;
  };
  case 'x': {
    uint32_t var = va_arg(*parameters, uint32_t);
    char numHex[16];
    int len = dec_to_hex(var, numHex);
    if (printText(numHex, len) == -1) {
      return -1;
    }
    return len;
  };
  default: {
    return -1;
  }
  }
}

int vprintf(const unsigned char *__restrict__ str, va_list parameters) {
  int i = 0;
  size_t written = 0;
  while (str[i] != '\0') {
    if (str[i] == '{' && str[i + 1] != '\0' && str[i + 2] == '}') {
      if (i > 0) {
        printText(str, i);
        str += i;
        written += i;
        i = 0;
      }

      int varLen = printVariable(str, &parameters);
      if (varLen == -1)
        return -1;

      str += 3;
      written += varLen;
    } else {
      i++;
    }
  }

  if (i > 0) {
    printText(str, i);
    written += i;
  }
  return written;
}
