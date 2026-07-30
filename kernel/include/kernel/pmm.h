#ifndef PHYSICAL_MEMORY_MANAGER_H
#define PHYSICAL_MEMORY_MANAGER_H 1

#include <kernel/mmap.h>
#include <multiboot2.h>
#include <stdint.h>
#include <stdlib.h>

#define PAGE_SIZE KB(4)

#define BIT_MAP_LEN UINT32_MAX / PAGE_SIZE / 8
extern uint8_t bit_map_allocator[BIT_MAP_LEN];

void pmm_init_bitmap();

#endif
