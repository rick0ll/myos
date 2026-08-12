
#ifndef _KERNEL_SLUB_H
#define _KERNEL_SLUB_H 1

#include <kernel/data_structures/list.h>
#include <stdbool.h>
#include <stdint.h>

// NUMA => Non-Uniform Memory Access è una divisione logica della RAM per
// permette a CPU diverse di usare parti diverse di memoria. CPU / core diversi
// sono in posizioni diverse dalla RAM, alcuni vicini altri lontani, se tutti
// chiedessero roba alla ram => intaso
//  divisione (CPU 0-3) chiede a questa parte di RAM con questo canale,(cpu 4-7)
//  chiede a un'altra con altro canale
#define MAX_NUMNODES 1

struct slub {
  void *freelist;
  struct list_head list;
  uint32_t obj_in_use;
};

struct kmem_cache_cpu {
  void **freelist;
  struct slub *slab;
  struct slub *partial;
};

struct kmem_cache_node {
  struct list_head *partial;
};

struct kmem_cache {
  struct kmem_cache_cpu *cpu_slab;
  struct kmem_cache_node *node[MAX_NUMNODES];
};

extern struct kmem_cache kmem_cache_16B;
extern struct kmem_cache kmem_cache_32B;
extern struct kmem_cache kmem_cache_64B;
extern struct kmem_cache kmem_cache_96B;
extern struct kmem_cache kmem_cache_128B;
extern struct kmem_cache kmem_cache_192B;
extern struct kmem_cache kmem_cache_256B;
extern struct kmem_cache kmem_cache_512B;
extern struct kmem_cache kmem_cache_1KB;
extern struct kmem_cache kmem_cache_2KB;

extern struct kmem_cache_node *node[MAX_NUMNODES];

#endif // !_KERNEL_SLUB_H
