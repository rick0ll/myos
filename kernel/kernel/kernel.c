#include <kernel/stack_protector.h>
#include <kernel/tty.h>

#include <stdint.h>
#include <stdio.h>

__attribute__((no_stack_protector)) uint32_t initCanary(void);

extern uint32_t __stack_chk_guard;

__attribute__((noinline)) void test(void) {
  // Un array da 16 byte per attivare lo stack protector
  volatile char buffer[16];
  buffer[0] = 'X';

  // Prendiamo l'indirizzo del buffer e lo camuffiamo in un puntatore generico.
  // Questo impedisce a GCC di capire che stiamo uscendo dai confini del buffer
  // durante la compilazione (zero warning!).
  volatile char *p = (volatile char *)((uintptr_t)buffer);

  // Andiamo a sovrascrivere i byte immediatamente successivi all'allineamento.
  // Su architetture a 32-bit, il canary si trova solitamente tra il byte 16 e
  // il byte 32. Scrivendo solo in questa finestrella, calpestiamo il Canary MA
  // lasciamo intatto il vecchio EBP e l'indirizzo di ritorno più in alto!
  p[16] = 0xAA;
  p[17] = 0xBB;
  /* p[18] = 0xCC; */
  /* p[19] = 0xDD; */
  /* p[20] = 0xEE; */
  /* p[21] = 0xFF; */
}

// Metto no_stack_protector pk quando il kernel è compilato
// il canary, inizializzato nel file stack_protector.c sarà diverso da
// quello verrà ora creato randomicamente, causando quindi un canary
// missmatch ed panic. Il kernel non ritorna mai ma per sicurezza.
__attribute__((no_stack_protector)) void kernel_main(void) {
  terminal_initialize();
  printf("Kernel avviato con successo!\n");

  // 2. Eseguiamo l'inizializzazione del canary DOPO
  printf("Inizializzo lo stack protector...\n");
  __stack_chk_guard = initCanary();
  printf("Stack protector inizializzato!\n");

  test();
  printf("test completato\n");
}

__attribute__((no_stack_protector)) uint32_t initCanary(void) {
  uint32_t val;

  // Esegue l'istruzione rdrand => num random in base a entropia hw. Se ha
  // successo, restituisce il valore. Altrimenti (raro) fallisce e restituisce
  // 0.
  //
  // volatile server per evitare che il compilatore ottimizzi il codice e
  // sposti la funzione
  //
  // serve tutto questo asm pk altrimenti solo rdtsc scrive il valore su EAX ed
  // EDX che sono importanti siccome vengono usati inizialmente per gestire lo
  // stack frame
  // asm volatile ( "codice" : output : input : clobber );
  asm volatile(
      "pushl %%eax\n\t"    // Salva EAX sullo stack
      "pushl %%edx\n\t"    // Salva EDX sullo stack
      "rdtsc\n\t"          // Esegue RDTSC (il tempo finisce in EDX:EAX)
      "movl %%eax, %0\n\t" // Sposta la parte bassa (EAX) nella variabile va val
                           // riferita con %0
      "popl %%edx\n\t"     // Ripristina EDX originale
      "popl %%eax"         // Ripristina EAX originale
      : "=r"(val)          // Output: una variabile in un registro qualsiasi
                  // scelto da GCC => viene scelto un registro che conterrà il
                  // dato che poi verrà movl nella variabile riferita con %0
      :          // Nessun input
      : "memory" // Dice a GCC di non fare assunzioni sulla memoria per evitare
                 // errori sui registri che abbiamo appena usato(clobber)
  );

  if (val == 0) {
    // siccome kernel per cpu 32bit e clock cpu di 64bit lo divide in due
    // parti
    uint32_t low, high;
    // Legge il contatore dei cicli di clock della CPU
    asm volatile("rdtsc" : "=a"(low), "=d"(high));

    // Mischia i bit con un'operazione XOR (es. usando i cicli di clock bassi)
    // siccome il clock parte sempre da 0 e la funzione viene eseguita subito
    // i valori di low, per quanto variabili, rimangono cmq in un range
    // limitato, con lo xor mischio tutto
    //
    // 0x55A storicamente è il Magic Number che i BIOS x86 cercano alla fine
    // della boot per verificare che sia valido. (Vedi boot.S)
    return (low ^ 0x55AA55AA) | 0x00;
  }

  return val | 0x00;
}
