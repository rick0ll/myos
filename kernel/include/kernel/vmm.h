
#ifndef _KERNEL_VMM_H
#define _KERNEL_VMM_H 1

#include <kernel/pmm.h>
#include <stdint.h>
#include <string.h>

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

/*
 * 31-12 physc_addr
 * 11-0 flags
 *
 * */
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
void vmm_map_page(uint32_t phys_addr, uint32_t virt_addr, uint32_t flags);
void vmm_unmap_page(uint32_t virt_addr);
static inline void __flush_tlb_single_page__(uint32_t pte_page_addr) {
  asm __volatile__("invlpg (%0)" : : "r"(pte_page_addr) : "memory");
}
#endif // _KERNEL_VMM_H
