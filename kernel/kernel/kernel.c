
#include <kernel/kernel.h>
// Metto no_stack_protector pk quando il kernel è compilato
// il canary, inizializzato nel file stack_protector.c sarà diverso da
// quello verrà ora creato randomicamente, causando quindi un canary
// missmatch ed panic. Il kernel non ritorna mai ma per sicurezza.
__attribute__((no_stack_protector)) void kernel_main(uintptr_t mb_addr) {
  __stack_chk_guard = initCanary();
  terminal_initialize();
  parse_info_request(mb_addr);
  pmm_init_bitmap();

  log_info("Kernel avviato con successo!\n");
}
