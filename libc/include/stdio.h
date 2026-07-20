#ifndef _STDIO_H
#define _STDIO_H 1

#include <sys/cdefs.h>

#define EOF (-1)
#define SWAP(T, a, b)                                                          \
  do {                                                                         \
    T temp = a;                                                                \
    a = b;                                                                     \
    b = temp;                                                                  \
  } while (0)

#ifdef __cplusplus
extern "C" {
#endif

int printf(const char *__restrict, ...);
int atoi(const char *);
int itoa(int, char[11]);
int putchar(int);
int puts(const char *);
int myprintf(const char *__restrict, ...);

#ifdef __cplusplus
}
#endif

#endif
