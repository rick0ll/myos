
// Il bios e l'hw x86 non mettono o usano la memoria a caso ma a blocchi
// la memoria è sempre multiplo di 32KB dunque l'array sarà sempre o 0x0 o 0xFF
#include <kernel/pmm.h>
#include <string.h>

uint8_t bit_map_allocator[BIT_MAP_LEN] = {0};
void pmm_init_bitmap() {

  extern uint32_t _kernel_start;
  extern uint32_t _kernel_end;

  struct multiboot_mmap_entry *entry;
  // Consideriamole tutte occupate e liberiamo poco alla volta
  // anche pk conviene, di base QEMU parte con 128MB RAM nonostante ci siano
  // disponibili 4GB, dunque la maggior parte della RAM non potrà essere usata
  // all'inizio, giustamente mettiamo a 1
  memset(bit_map_allocator, UINT8_MAX, BIT_MAP_LEN);
  uint32_t num = get_mmap_entries_num();

  for (uint32_t i = 0; i < num; i++) {
    entry = &tag_mmap->entries[i];
    if (entry->type != MULTIBOOT_MEMORY_AVAILABLE)
      continue;

    if (entry->length < PAGE_SIZE)
      continue;

    uint32_t alligned_base_addr =
        MULTIPLO_PER_ECCESSO(entry->base_addr, PAGE_SIZE);
    uint32_t padding = alligned_base_addr - entry->base_addr;
    uint32_t len = entry->length - padding;

    do {
      size_t all_bits_index = alligned_base_addr / PAGE_SIZE;

      /*
       * CALCOLO DEL RESTO SENZA OPERATORE '%' (OTTIMIZZAZIONE BITWISE)
       *
       * Formula: dividendo & (divisore - 1)
       * Condizione: Funziona SOLO se il divisore è una potenza di 2
       * (es. 2, 4, 8, 16...).
       *
       * Come funziona per "12 % 4":
       * 1. Il divisore meno uno (4 - 1 = 3) crea una maschera di bit: 3
       * in binario è 0011.
       * 2. L'operatore AND (&) isola i bit a destra che rappresentano
       * il resto
       * (< 4), azzerando istantaneamente tutti i bit che valgono 4 o
       * più.
       *
       * Rappresentazione binaria:
       *    12  -->  1 1 0 0
       *   & 3  -->  0 0 1 1
       *   -----------------
       *    0   -->  0 0 0 0  (Il resto è 0)
       *
       * Prestazioni: Eseguito in 1 solo ciclo di clock della CPU (molto
       * più veloce di %).
       */
      uint8_t bit_index = all_bits_index % 8;

      uint32_t array_index = all_bits_index / 8;

      // 1 => occupato
      // 0 => libero
      bit_map_allocator[array_index] |= (1 << bit_index);
      alligned_base_addr += PAGE_SIZE;
      len -= PAGE_SIZE;
    } while (len >= PAGE_SIZE);
  }

  uint32_t kernel_end_page =
      MULTIPLO_PER_ECCESSO((uint32_t)&_kernel_end, PAGE_SIZE) / PAGE_SIZE;

  uint32_t kernel_start_page = _kernel_start / PAGE_SIZE;

  for (uint32_t i = kernel_start_page; i < kernel_end_page; i++) {
    uint32_t array_index = i / 8;
    uint8_t bit_index = i % 8;

    bit_map_allocator[array_index] |= (1 << bit_index);
  }
}
