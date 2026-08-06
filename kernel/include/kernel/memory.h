#ifndef KERNEL_MEMORY_H
#define KERNEL_MEMORY_H 1

#define KERNEL_VIRT_BASE 0xC0000000

#ifndef __ASSEMBLER__

#include <stdint.h>

#define PHYS_TO_VIRT(phys_addr) ((uint32_t)(phys_addr) + KERNEL_VIRT_BASE)
#define VIRT_TO_PHYS(virt_addr) ((uint32_t)(virt_addr) - KERNEL_VIRT_BASE

#endif // !__ASSEMBLER__
#endif
