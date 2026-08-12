
// Il bios e l'hw x86 non mettono o usano la memoria a caso ma a blocchi
// la memoria è sempre multiplo di 32KB dunque l'array sarà sempre o 0x0 o 0xFF
#include "kernel/memory.h"
#include <kernel/pmm.h>

static uint32_t last_allocated_array_index = 0;
uint8_t bit_map_allocator[BIT_MAP_LEN];

uint8_t pmm_free_page(uint32_t address) {
  if (address == 0 || address % PAGE_SIZE) {
    return 1;
  }

  uint32_t bitmap_bit_index = address / PAGE_SIZE;

  uint32_t array_index = bitmap_bit_index / 8;
  uint8_t bit_position = bitmap_bit_index % 8;
  if (array_index >= BIT_MAP_LEN)
    return 1;

  if (array_index < last_allocated_array_index) {
    last_allocated_array_index = array_index;
  }
  bit_map_allocator[array_index] &= ~(1 << bit_position);
  return 0;
}

// return first 4KB alligned  physical addr available setted to 0 else returns 0
uint32_t pmm_alloc_page(void) {

  for (uint32_t array_index = last_allocated_array_index;
       array_index < BIT_MAP_LEN; array_index++) {

    if (bit_map_allocator[array_index] == 0xFF)
      continue;

    for (uint8_t bit_position = 0; bit_position < 8; bit_position++) {
      if (is_bitmap_entry_occupato(array_index, bit_position))
        continue;

      bit_map_allocator[array_index] |= (1 << bit_position);

      uint32_t frame_index = (array_index * 8) + bit_position;
      uint32_t phys_addr = frame_index * PAGE_SIZE;

      last_allocated_array_index = array_index;

      return phys_addr;
    }
  }
  log_warn("no pmm allocation");
  return 0x0;
}

void pmm_init(void) {
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

    if (len < PAGE_SIZE)
      continue;

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
      // se oltre 4GB per ora
      if (array_index >= BIT_MAP_LEN)
        break;
      bit_map_allocator[array_index] &= ~(1 << bit_index);
      alligned_base_addr += PAGE_SIZE;
      len -= PAGE_SIZE;
    } while (len >= PAGE_SIZE);
  }

  uint32_t kernel_end_bit_position =
      MULTIPLO_PER_ECCESSO((uint32_t)VIRT_TO_PHYS(&_kernel_end), PAGE_SIZE) /
      PAGE_SIZE);

  uint32_t kernel_end_bitmap_index =
      MULTIPLO_PER_ECCESSO(kernel_end_bit_position, 8) / 8;

  // rendo da 0 - kernel end occupato => primo 1MB pe hw , resto è kernel
  memset(bit_map_allocator, UINT8_MAX, kernel_end_bitmap_index);
}
