
#include "kernel/logger.h"
#include <kernel/kernel.h>
// il canary, inizializzato nel file stack_protector.c sarà diverso da
// quello verrà ora creato randomicamente, causando quindi un canary
// missmatch ed panic. Il kernel non ritorna mai ma per sicurezza.
__attribute__((no_stack_protector)) void kernel_main(uintptr_t mb_addr) {
  __stack_chk_guard = initCanary();
  terminal_initialize();
  parse_info_request(mb_addr);
  pmm_init_bitmap();

  /* extern uint32_t _kernel_end; */
  /* log_trace("Kernel end unaligned: {x}\n", (uint32_t)&_kernel_end); */
  /* log_trace("Kernel end: {x}\n", */
  /*           (uint32_t)MULTIPLO_PER_ECCESSO(&_kernel_end, KB(4))); */
  /**/
  /* uint32_t addr = pmm_alloc_page(); */
  /* log_trace("Addr: {x}\n", addr); */
  /* log_trace("End: {x}\n", addr + KB(4)); */
  /**/
  /* uint8_t ok = pmm_free_page(addr); */
  /* log_trace("Free?: {d}\n", ok); */

  log_info("Kernel avviato con successo!");
}
