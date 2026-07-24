#include <stdint.h>
#ifndef _STDLIB_H
#define _STDLIB_H 1

#include <sys/cdefs.h>

#ifdef __cplusplus
extern "C" {
#endif

#define SWAP(T, a, b)                                                          \
  do {                                                                         \
    T temp = a;                                                                \
    a = b;                                                                     \
    b = temp;                                                                  \
  } while (0)

__attribute__((__noreturn__)) void abort(void);
void panic(const char *);

int atoi(const char *);
int itoa(int, char[11]);
void dec_to_hex(uint32_t num, char hex[11]);

#ifdef __cplusplus
}
#endif

#endif
