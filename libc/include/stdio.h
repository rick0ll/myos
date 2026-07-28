#ifndef _STDIO_H
#define _STDIO_H 1

#include <stdarg.h>
#include <sys/cdefs.h>

#define EOF (-1)
#ifdef __cplusplus
extern "C" {
#endif

// int old_printf(const char *__restrict, ...);
int putchar(const unsigned char);
int puts(const unsigned char *);
int printf(const unsigned char *__restrict, ...);
int vprintf(const unsigned char *__restrict, va_list);

#ifdef __cplusplus
}
#endif

#endif
