#ifndef _STDLIB_H
#define _STDLIB_H 1

#include <stdint.h>
#include <sys/cdefs.h>

#ifdef __cplusplus
extern "C" {
#endif

#define MULTIPLO_PER_ECCESSO(NUM, SIZE)                                        \
  (((size_t)(NUM) + (SIZE - 1)) & ~(SIZE - 1))

#define MULTIPLO_PER_DIFETTO(NUM, SIZE) ((size_t)(NUM) & (SIZE - 1))

#define KB(x) ((size_t)(x) << 10)
#define MB(x) ((size_t)(x) << 20)

#define SWAP(T, a, b)                                                          \
  do {                                                                         \
    T temp = a;                                                                \
    a = b;                                                                     \
    b = temp;                                                                  \
  } while (0)

__attribute__((__noreturn__)) void abort(void);
void panic(const char *);

int atoi(const unsigned char *);
int itoa(int, unsigned char[11]);
int dec_to_hex(uint32_t num, unsigned char *hex);

#ifdef __cplusplus
}
#endif

#endif
