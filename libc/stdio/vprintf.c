#include <limits.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int printText(const char *str, size_t len) {
  if (len > INT_MAX)
    return -1;

  const unsigned char *buff = (const unsigned char *)str;

  for (size_t i = 0; i < len; i++) {
    if (putchar(buff[i]) == EOF)
      return -1;
  }

  return len;
}

int printVariable(const char *__restrict__ str, va_list parameters) {
  int i = 0;
  char type = str[i + 1];

  switch (type) {
  case 's': {
    const char *var = (const char *)va_arg(parameters, const char *);
    size_t len = strlen(var);
    if (printText(var, len) == -1) {
      return -1;
    }
    return len;
  }; break;

  case 'c': {
    char var = (char)va_arg(parameters, int);
    if (printText(&var, 1) == -1) {
      return -1;
    }
    return 1;
  }; break;

  case 'd': {
    int var = (int)va_arg(parameters, int);
    char num[11];
    int len = itoa(var, num);
    if (printText(num, len) == -1) {
      return -1;
    }
    return len;
  }; break;

  case 'x': {
    uint32_t var = (uint32_t)va_arg(parameters, uint32_t);
    char numHex[11];
    dec_to_hex(var, numHex);
    if (printText(numHex, 10) == -1) {
      return -1;
    }
    return 10;
  }; break;
  }
  return -1;
}

int vprintf(const char *__restrict__ str, va_list parameters) {
  const char *tmp = str;
  int i = 0;
  size_t written = 0;
  while (str[i] != '\0') {
    if (str[i] == '{' && str[i + 1] != '\0' && str[i + 2] == '}') {
      printText(str, i);
      str += i;
      i = 0;

      int varLen = printVariable(str, parameters);
      str += 3;

      if (varLen == -1)
        return -1;

      written += varLen;
    } else {
      i++;
    }
  }

  printText(str, i);
  str = tmp;
  return written;
}
