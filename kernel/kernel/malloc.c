
#include <kernel/malloc.h>

struct cache_entry kmalloc_caches[SLUB_COUNT];
uint32_t first_virt_addr = 0;

int index_from_size(uint32_t size) {
  if (size > KB(2))
    return BYTE_4K;
  uint32_t byte_size = 1 << 3;
  for (enum obj_type_t i = 0; i < SLUB_COUNT - 1; i++) {
    if (size < byte_size) {
      return i;
    }
    byte_size <<= 1;
  }
  return -1;
}

int init_freelist(struct slab_t *slab, enum obj_type_t obj_type) {
  if (obj_type == BYTE_4K)
    return 0;
  if (slab == NULL || obj_type < BYTE_8)
    return -1;
  // aritmetica di puntatori void* la somma è come sommare 1byte pk sizeof(void)
  // = 1B
  void *start = (void *)slab + sizeof(struct slab_t);

  MULTIPLO_PER_ECCESSO(start, 0x10);

  void *end = (void *)slab + PAGE_SIZE;

  slab->freelist = (void *)start;

  uint32_t obj_size = OBJS_SIZE(obj_type);
  void *curr = start;

  while (curr + 2 * obj_size < end) {
    void *next = curr + obj_size;

    // scirvo nei primi 4byte di curr il valore di next
    *(uint32_t *)curr = (uint32_t)next;
    // come fare, più C-style
    //*(void **)curr = next;

    curr = next;
  }

  *(uint32_t *)curr = 0;
  return 0;
}

struct slab_t *new_slab(struct cache_entry *cache, enum obj_type_t type) {
  if (!cache)
    return NULL;

  struct slab_t *slab = (struct slab_t *)vmm_alloc_page(KERNEL_STD_PAGE_FLAGS);
  if (!slab)
    return NULL;

  slab->cache = cache;
  slab->inuse = 0;
  slab->next = cache->mem;

  if (cache->mem != NULL)
    cache->mem->prev = slab;

  slab->prev = slab;
  cache->mem = slab;

  if (init_freelist(slab, type) == -1)
    return NULL;

  return slab;
}

void *kmalloc(uint32_t size) {
  enum obj_type_t index = index_from_size(size);
  if ((int)index == -1 || size >= KB(4))
    return NULL;

  struct cache_entry *cache = &kmalloc_caches[index];
  struct slab_t *slab = cache->mem;

  while (slab != NULL && slab->freelist == NULL) {
    slab = slab->next;
  }

  if (slab == NULL) {
    slab = new_slab(cache, index);
    if (slab == NULL)
      return NULL;
  }

  void *addr = slab->freelist;
  slab->freelist = LIST_NEXT(slab->freelist);
  slab->inuse++;
  return addr;
}

int free_slab(struct slab_t *slab) {
  if (slab->cache->mem == slab) {
    // CASO A: Era il primo elemento
    slab->cache->mem = slab->next;
    if (slab->next != NULL) {
      slab->next->prev =
          slab->next; // Il nuovo primo elemento punta a se stesso
    }
  } else {
    // CASO B: Era in mezzo o in fondo
    slab->prev->next = slab->next;
    if (slab->next != NULL) {
      slab->next->prev = slab->prev;
    }
  }
  if (vmm_unmap_page((uint32_t)slab) == -1) {
    return -1;
  }
  return 0;
}

int kfree(void *ptr) {
  if (!ptr)
    return 0;
  struct slab_t *slab = (struct slab_t *)((uint32_t)ptr & ~(PAGE_SIZE - 1));

  *(uint32_t *)ptr = (uint32_t)slab->freelist;
  slab->freelist = ptr;

  slab->inuse--;
  if (slab->inuse == 0) {
    return free_slab(slab);
  }
  return 0;
}

void init_kmalloc() {
  for (enum obj_type_t i = 0; i < SLUB_COUNT; i++) {
    kmalloc_caches[i].obj_type = i;
    kmalloc_caches[i].mem = NULL;
  }
}
