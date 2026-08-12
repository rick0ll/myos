
#ifndef _KERNEL_MALLOC_H
#define _KERNEL_MALLOC_H 1

#include <kernel/vmm.h>
#include <stdbool.h>
#include <stdint.h>

enum obj_type_t {
  BYTE_8 = 0,
  BYTE_16,
  BYTE_32,
  BYTE_64,
  // BYTE_96,
  BYTE_128,
  BYTE_256,
  BYTE_512,
  BYTE_1K,
  BYTE_2K,
  BYTE_4K,
  SLUB_COUNT,
};

#define MAX_SLAB_OBJS_NUM(obj_type) PAGE_SIZE / (8 << (obj_type))
#define OBJS_SIZE(obj_type) 8 << (obj_type)
#define LIST_NEXT(list) ((void *)(*(uint32_t *)(list)))

struct slab_t {
  void *freelist;
  uint32_t inuse;
  struct slab_t *next, *prev;
  struct cache_entry *cache;
};

struct cache_entry {
  struct slab_t *mem;
  enum obj_type_t obj_type;
};

void *kmalloc(uint32_t size);
int kfree(void *ptr);
void init_kmalloc();

#endif // !_KERNEL_MALLOC_H
