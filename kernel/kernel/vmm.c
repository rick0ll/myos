
#include <kernel/vmm.h>

uint32_t active_page_table_entries[1024] = {0};
page_directory_t *page_directory_ptr = NULL;

void vmm_enable_paging(uint32_t page_directory_addr);

void vmm_map_page(uint32_t phys_addr, uint32_t virt_addr, uint32_t flags) {
  if (phys_addr % KB(4) != 0) {
    log_error("phys_addr is not 4KB alligned. phys_addr: {x}", phys_addr);
    return;
  }
  if (virt_addr % KB(4) != 0) {
    log_error("virt_addr is not 4KB alligned. virt_addr: {x}", virt_addr);
    return;
  }

  // MB(4) pk ogni pde index punta a una pte che sono 1024 x 4KB => 4MB
  uint32_t pde_index = virt_addr / MB(4);
  if ((page_directory_ptr->entries[pde_index] & PE_PRESENT_FLAG) !=
      PE_PRESENT_FLAG) {
    page_directory_ptr->entries[pde_index] =
        pmm_alloc_page() | PE_PRESENT_FLAG | PE_RW_FLAG;
  }

  page_table_t *page_table =
      (page_table_t *)(page_directory_ptr->entries[pde_index] & ONLY_ADDR);

  uint32_t pte_index = (virt_addr / KB(4)) % 1024;
  page_table->entries[pte_index] = phys_addr | flags | PE_PRESENT_FLAG;
  __flush_tlb_single_page__(virt_addr);

  active_page_table_entries[pde_index]++;
}

void vmm_unmap_page(uint32_t virt_addr) {
  if (virt_addr % KB(4) != 0) {
    log_error("virt_addr is not 4KB alligned. virt_addr: {x}", virt_addr);
    return;
  }

  uint32_t pde_index = virt_addr / MB(4);
  if ((page_directory_ptr->entries[pde_index] & PE_PRESENT_FLAG) !=
      PE_PRESENT_FLAG) {
    log_trace("la pagine da unmap non è presente nella page directory. "
              "virt_addr: {x}",
              virt_addr);
    return;
  }

  page_table_t *page_table =
      (page_table_t *)(page_directory_ptr->entries[pde_index] & ONLY_ADDR);

  uint32_t pte_index = (virt_addr / KB(4)) % 1024;
  if ((page_table->entries[pte_index] & PE_PRESENT_FLAG) != PE_PRESENT_FLAG) {
    log_trace("la pagine da unmap non è presente nella page table. "
              "virt_addr: {x}",
              virt_addr);
    return;
  }

  pmm_free_page(page_table->entries[pde_index] & ONLY_ADDR);
  page_table->entries[pte_index] = 0;

  // invalido la cache per questa pagina siccome unmapped
  __flush_tlb_single_page__(virt_addr);

  active_page_table_entries[pde_index]--;
  // se per una pagina non ci sono più entry attive (in uso) allora dico che
  // la page directory entry relativa a questa pagina e vuota, non in uso,
  // dovrà essere riallocata
  if (active_page_table_entries[pde_index] <= 0) {
    pmm_free_page(page_directory_ptr->entries[pde_index] & ONLY_ADDR);
    page_directory_ptr->entries[pde_index] = 0;
  }
}

void vmm_init() {
  page_directory_ptr = (page_directory_t *)pmm_alloc_page();

  page_table_t *page_table_ptr = (page_table_t *)pmm_alloc_page();

  // Mappo in modo 1:1 i primi 4MB => 1024 * 4KB => quando la cpu attiverà la
  // memoria virtuale vedrà i suoi indirizzi di esecuzione kernel normali e
  // non virtuali ma senza corrispondenza fisica, serve all'inizio del setup
  // vmm
  for (int i = 0; i < PTE_ENTRIES_NUM; i++) {
    page_table_ptr->entries[i] = (i * PAGE_SIZE) | PE_PRESENT_FLAG | PE_RW_FLAG;
  }

  page_directory_ptr->entries[0] =
      ((uint32_t)page_table_ptr) | PE_PRESENT_FLAG | PE_RW_FLAG;

  vmm_enable_paging((uint32_t)page_directory_ptr);
}

void vmm_enable_paging(uint32_t page_directory_addr) {
  // metto su CR3 la prima variabile che gli passo ossia indirizzo PDT
  __asm__ volatile("mov %0, %%cr3\n\t"
                   // Siccome non posso fare operazioni or direttamente su cr0
                   "mov %%cr0, %%eax\n\t"
                   // or 10000..00 pongo a 1 il bit 31^ ossia enable paging
                   "or $0x80000000, %%eax\n\t"
                   // rimetto su CR0
                   "mov %%eax, %%cr0\n\t"
                   :
                   // dice al compiler di mettere questa var in un registro
                   // così che %0 lo possa leggere
                   : "r"(page_directory_addr)
                   // dico al compilatore che uso il registro eax  e sporco
                   // anche memoria dunque di non aspettarsi che abbia il
                   // valore precedente al mio dunque ignora il valore di eax
                   // e ricarica dalla ram il valore precedente
                   : "%eax", "memory");
}
