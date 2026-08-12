#include <kernel/kernel.h>

// il canary, inizializzato nel file stack_protector.c sarà diverso da
// quello verrà ora creato randomicamente, causando quindi un canary
// missmatch ed panic. Il kernel non ritorna mai ma per sicurezza.
__attribute__((no_stack_protector)) void kernel_main(uintptr_t mb_addr) {
  __stack_chk_guard = initCanary();
  terminal_initialize();
  parse_info_request(PHYS_TO_VIRT(mb_addr));
  pmm_init();
  vmm_init();
  init_kmalloc();

  log_info("Kernel avviato con successo!");
}
