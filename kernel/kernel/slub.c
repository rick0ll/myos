

#include <kernel/slub.h>
#include <stddef.h>

void *kmalloc(size_t size) {}

void init_malloc() {

  struct kmem_cache kmem_cache_8B;
  kmem_cache_8B.node = struct;
}
