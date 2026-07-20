#include <kernel/stack_protector.h>
#include <kernel/tty.h>

#include <stdint.h>
#include <stdio.h>

__attribute__((no_stack_protector)) uint32_t initCanary(void);

// Metto no_stack_protector pk quando il kernel è compilato
// il canary, inizializzato nel file stack_protector.c sarà diverso da
// quello verrà ora creato randomicamente, causando quindi un canary
// missmatch ed panic. Il kernel non ritorna mai ma per sicurezza.
__attribute__((no_stack_protector)) void kernel_main(void) {
  __stack_chk_guard = initCanary();
  terminal_initialize();

  printf("Stack protector inizializzato!\n");
  printf("Kernel avviato con successo!\n");
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
