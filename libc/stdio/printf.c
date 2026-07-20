#include <limits.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>

int print(const char *str, size_t len) {
  if (len > INT_MAX)
    return -1;

  const unsigned char *buff = (const unsigned char *)str;
  for (size_t i = 0; i < len; i++)
    if (putchar(buff[i]) == EOF)
      return -1;

  return len;
}

int printVariable(const char *__restrict__ str, va_list parameters) {
  int i = 0;
  char type = str[i + 1];

  switch (type) {
  case 's': {
    const char *var = (const char *)va_arg(parameters, const char *);
    size_t len = strlen(var);
    if (print(var, len) == -1) {
      return -1;
    }
    return len;
  }; break;

  case 'c': {
    char var = (char)va_arg(parameters, int);
    if (print(&var, 1) == -1) {
      return -1;
    }
    return 1;
  }; break;

  case 'd': {
    int var = (int)va_arg(parameters, int);
    char num[11];
    int len = itoa(var, num);
    if (print(num, len) == -1) {
      return -1;
    }

    //-1 bc it counts \0 as well
    return len - 1;
  }; break;
  }

  return -1;
}

/* "ciao {s}", str */
int printf(const char *__restrict__ str, ...) {
  va_list parameters;
  va_start(parameters, str);

  const char *tmp = str;
  int i = 0;
  size_t written = 0;

  while (str[i] != '\0') {
    if (str[i] == '{' && str[i + 1] != '\0' && str[i + 2] == '}') {
      print(str, i);
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
  print(str, i);
  str = tmp;
  return written;
}
