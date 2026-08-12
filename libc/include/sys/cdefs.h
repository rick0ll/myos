#ifndef _SYS_CDEFS_H
#define _SYS_CDEFS_H 1

#define __loss_libc 1

#define NR_CPUS 32

#if defined(__is_myos_kernel) || defined(__is_libk)
#define __KERNEL__ 1
#endif

#endif
