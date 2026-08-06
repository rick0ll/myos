#ifndef _STRING_H
#define _STRING_H 1

#include <sys/cdefs.h>

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

int memcmp(const void *, const void *, size_t);
void *memcpy(void *__restrict, const void *__restrict, size_t);
void *memmove(void *, const void *, size_t);
void *memset(void *dest, int value, size_t length);
size_t strlen(const char *);
char *strcpy(unsigned char *__restrict__ dst,
             unsigned const char *__restrict__ text);

#ifdef __cplusplus
}
#endif

#endif
