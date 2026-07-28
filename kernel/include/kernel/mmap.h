
#ifndef KERNEL_MMAP_H
#define KERNEL_MMAP_H 1

#include <multiboot2.h>
#include <stddef.h>
#include <stdint.h>

extern struct multiboot_tag_mmap *tag_mmap;
uint32_t get_mmap_entries_num();

#endif
