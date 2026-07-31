#ifndef PHYSICAL_MEMORY_MANAGER_H
#define PHYSICAL_MEMORY_MANAGER_H 1

#include <kernel/mmap.h>
#include <multiboot2.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#define PAGE_SIZE KB(4)

#define BIT_MAP_LEN UINT32_MAX / PAGE_SIZE / 8
// primi 32 entries => 32 entries * 8 bit / entries  * 4KB/bit = 1024KB =
// 1MB settati a 1 pk imporatnte per hw/bios etc
#define BITMAP_1MB_INDEX 32

extern uint8_t bit_map_allocator[BIT_MAP_LEN];

static inline bool is_bitmap_entry_occupato(uint32_t array_index,
                                            uint8_t bit_position) {
  return bit_map_allocator[array_index] & (1 << bit_position);
}

uint8_t pmm_free_page(uint32_t address);
void pmm_init_bitmap();
uint32_t pmm_alloc_page();

#endif
