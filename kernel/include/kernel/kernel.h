

#include <kernel/gdt.h>
#include <kernel/logger.h>
#include <kernel/malloc.h>
#include <kernel/memory.h>
#include <kernel/mmap.h>
#include <kernel/panic.h>
#include <kernel/parse_info_requests.h>
#include <kernel/pmm.h>
#include <kernel/stack_protector.h>
#include <kernel/tty.h>
#include <kernel/vmm.h>

#include <stdint.h>

void kernel_main(uintptr_t mb_addr);
