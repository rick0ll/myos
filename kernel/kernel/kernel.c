

#include <kernel/kernel.h>
// il canary, inizializzato nel file stack_protector.c sarà diverso da
// quello verrà ora creato randomicamente, causando quindi un canary
// missmatch ed panic. Il kernel non ritorna mai ma per sicurezza.
__attribute__((no_stack_protector)) void kernel_main(uintptr_t mb_addr) {
  __stack_chk_guard = initCanary();
  terminal_initialize();
  parse_info_request(mb_addr);
  pmm_init();
  vmm_init();

  // 1. Chiediamo una pagina fisica al PMM
  uint32_t phys_addr = pmm_alloc_page();
  log_info("1. Phys addr da PMM: {x}", phys_addr);

  // 2. La mappiamo all'indirizzo virtuale 0x0567000 dandole i permessi di
  // SCRITTURA (PE_RW)!
  vmm_map_page(phys_addr, 0x0567000, PE_RW_FLAG);

  // 3. Creiamo il puntatore al nostro indirizzo virtuale
  char *testo = (char *)0x0567000;

  // 4. Scriviamo DAVVERO dentro la RAM mappata a quell'indirizzo virtuale
  strcpy(testo, "ciao");

  // 5. Logghiamo la stringa e l'indirizzo
  log_info("frase: {s}. addr: {x}", testo, testo);

  log_info("Kernel avviato con successo!");
}
