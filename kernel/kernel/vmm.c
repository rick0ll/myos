
#include <kernel/vmm.h>

uint32_t active_page_table_entries[1024] = {0};
page_directory_t *page_directory_ptr = NULL;
uint32_t kheap_virt_curr = KHEAP_START;

uint32_t kheap_next_free_virt_addr() {
  uint32_t addr = kheap_virt_curr;
  kheap_virt_curr += PAGE_SIZE;
  return addr;
}

void *vmm_alloc_page(uint32_t flags) {

  uint32_t phys_addr = pmm_alloc_page();
  if (!phys_addr)
    return NULL;

  uint32_t virt_addr = kheap_next_free_virt_addr();

  if (vmm_map_page(phys_addr, virt_addr, flags) == -1) {

    return NULL;
  }

  return (void *)virt_addr;
}

int vmm_map_page(uint32_t phys_addr, uint32_t virt_addr, uint32_t flags) {

  CHECK_PAGE_ALLIGNMENT(phys_addr);
  CHECK_PAGE_ALLIGNMENT(virt_addr);

  // MB(4) pk ogni pde index punta a una pte che sono 1024 x 4KB => 4MB
  uint32_t pde_index = virt_addr / MB(4);

  if (!IS_THERE_BIT(page_directory_ptr->entries[pde_index], PE_PRESENT_FLAG)) {

    void *phys_addr = (void *)pmm_alloc_page();
    if (phys_addr == NULL)
      return -1;

    void *virt_addr = phys_addr + 0xC0000000;
    memset(virt_addr, 0, PAGE_SIZE);

    page_directory_ptr->entries[pde_index] =
        (uint32_t)phys_addr | PE_PRESENT_FLAG | PE_RW_FLAG;
  }

  uint32_t pt_phys_addr = page_directory_ptr->entries[pde_index] & ONLY_ADDR;
  page_table_t *page_table = (page_table_t *)(pt_phys_addr + 0xC0000000);

  uint32_t pte_index = (virt_addr / KB(4)) % 1024;
  page_table->entries[pte_index] = phys_addr | flags | PE_PRESENT_FLAG;
  __flush_tlb_single_page__(virt_addr);

  active_page_table_entries[pde_index]++;
  return 0;
}

int vmm_unmap_page(uint32_t virt_addr) {
  CHECK_PAGE_ALLIGNMENT(virt_addr);

  uint32_t pde_index = virt_addr / MB(4);
  if (!IS_THERE_BIT(page_directory_ptr->entries[pde_index], PE_PRESENT_FLAG)) {
    log_trace("la pagine da unmap non è presente nella page directory. "
              "virt_addr: {x}",
              virt_addr);
    return -1;
  }

  uint32_t pt_phys = page_directory_ptr->entries[pde_index] & ONLY_ADDR;
  page_table_t *page_table = (page_table_t *)(pt_phys + 0xC0000000);
  uint32_t pte_index = (virt_addr / KB(4)) % 1024;
  if (!IS_THERE_BIT(page_table->entries[pte_index], PE_PRESENT_FLAG)) {
    log_trace("la pagine da unmap non è presente nella page table. "
              "virt_addr: {x}",
              virt_addr);
    return -1;
  }

  pmm_free_page(page_table->entries[pte_index] & ONLY_ADDR);
  page_table->entries[pte_index] = 0;

  // invalido la cache per questa pagina siccome unmapped
  __flush_tlb_single_page__(virt_addr);

  active_page_table_entries[pde_index]--;
  // se per una pagina non ci sono più entry attive (in uso) allora dico che
  // la page directory entry relativa a questa pagina e vuota, non in uso,
  // dovrà essere riallocata
  if (active_page_table_entries[pde_index] == 0) {
    pmm_free_page(page_directory_ptr->entries[pde_index] & ONLY_ADDR);
    page_directory_ptr->entries[pde_index] = 0;
  }
  return 0;
}

void vmm_init() {
  uint32_t cr3_phys;
  // Leggiamo l'indirizzo fisico della directory attualmente in uso dalla CPU
  asm volatile("mov %%cr3, %0" : "=r"(cr3_phys));

  // Lo convertiamo in virtuale affinché il kernel possa modificarlo
  page_directory_ptr = (page_directory_t *)(cr3_phys + 0xC0000000);
  /* void *phys_addr = (void *)pmm_alloc_page(); */
  /* void *virt_addr = phys_addr + 0xC0000000; */
  /* memset(virt_addr, 0, PAGE_SIZE); */
  /* page_directory_ptr = (page_directory_t *)virt_addr; */
}
