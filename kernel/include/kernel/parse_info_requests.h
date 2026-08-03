
#ifndef _KERNEL_PARSE_INFO_REQ
#define _KERNEL_PARSE_INFO_REQ 1

#include <kernel/logger.h>
#include <kernel/mmap.h>
#include <multiboot2.h>
#include <stdint.h>

void parse_info_request(uintptr_t);

#endif
