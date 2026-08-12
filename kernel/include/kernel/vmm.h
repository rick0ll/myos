

#ifndef _KERNEL_VMM_H
#define _KERNEL_VMM_H 1

#define PTE_MEM_SIZE KB(4)
#define PTE_MEM_SIZE_BIG MB(4)

#define PDE_ENTRY_SIZE MB(4)

#define ONLY_ADDR 0xFFFFF000

#define PE_PRESENT_FLAG (1 << 0)
#define PE_RW_FLAG (1 << 1)
#define PE_USER_FLAG (1 << 2)
#define PE_PAGE_WRITE_THROUGH_FLAG (1 << 3)
#define PE_CACHE_DISABLED_FLAG (1 << 4)

#define PDE_MEM_SIZE_4MB_ENTRY_FLAG (1 << 7)
#define PTE_PAT_FLAG (1 << 7)

#define PDE_4MB_GLOBAL_FLAG (1 << 8)
#define PTE_GLOBAL_FLAG (1 << 8)

#define PDR_4MB_PAT_FLAG (1 << 12)
#define PDR_4MB_RSVD_FLAG 0

#define PDE_ENTRIES_NUM 1024
#define PTE_ENTRIES_NUM 1024

#define KERNEL_PAGE_BOOT                                                       \
  0x0 | PE_PRESENT_FLAG | PE_RW_FLAG | PDE_MEM_SIZE_4MB_ENTRY_FLAG

/*                                                     \
 * 31-12 physc_addr                                    \
 * 11-0 flags                                          \
 *                                                     \
 * */

#ifndef __ASSEMBLER__

#include <kernel/memory.h>
#include <kernel/pmm.h>
#include <stdint.h>

#define KERNEL_STD_PAGE_FLAGS 0x0 | PE_PRESENT_FLAG | PE_RW_FLAG

#define KHEAP_START KERNEL_VIRT_BASE + 0x00400000

#define CHECK_PAGE_ALLIGNMENT(addr)                                            \
  do {                                                                         \
    if (addr % KB(4) != 0) {                                                   \
      log_error("{s} is not 4KB alligned. virt_addr: {x}", #addr, addr);       \
      return -1;                                                               \
    }                                                                          \
  } while (0)

#define IS_THERE_BIT(obj, bit) (((obj) & (bit)) == (bit))

typedef uint32_t page_table_entry_t;
typedef struct page_table_t {
  page_table_entry_t entries[PTE_ENTRIES_NUM];
} page_table_t;

// uguale ma invece che physc_addr page_table addr
typedef uint32_t page_directory_entry_t;
typedef struct page_directory_t {
  page_directory_entry_t entries[PDE_ENTRIES_NUM];
} page_directory_t;

void vmm_init();
void *vmm_alloc_page(uint32_t flags);
int vmm_map_page(uint32_t phys_addr, uint32_t virt_addr, uint32_t flags);
int vmm_unmap_page(uint32_t virt_addr);
static inline void __flush_tlb_single_page__(uint32_t pte_page_addr) {
  asm __volatile__("invlpg (%0)" : : "r"(pte_page_addr) : "memory");
}
static inline void __invalidate_tlb_cache(void) {
  asm volatile("mov %%cr3, %%eax\n\t"
               "mov %%eax, %%cr3\n\t" ::
                   : "eax", "memory");
}

#endif
#endif
