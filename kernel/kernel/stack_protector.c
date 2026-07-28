#include <kernel/tty.h>
#include <stddef.h>
#include <stdint.h>

#define STACK_CHK_GUARD 0xe2dee396

uint32_t __stack_chk_guard = STACK_CHK_GUARD;

__attribute__((noreturn)) void __stack_chk_fail(void) {
  // meglio non chiamare panic pk se sono in questa func => stack compromesso,
  // meglio non usarlo
  /* panic("Stack smashing detected"); */
  uint16_t *video_mem = (uint16_t *)0xb8000;

  uint16_t blank_pixel = (uint16_t)' ' | (0x1F << 8);
  for (size_t i = 0; i < 80 * 25; i++) {
    video_mem[i] = blank_pixel;
  }

  const char *msg = "Stack smashing detected";
  for (size_t i = 0; msg[i] != '\0'; i++) {
    video_mem[i] = msg[i] | 0x4F00; // 0x4F00 è il colore rosso
  }

  while (1) {
    asm volatile("cli; hlt");
  }
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
  // udcloud
  // EDX che sono importanti siccome vengono usati inizialmente per gestire lo
  // stack frame
  // asm volatile ( "codice" : output : input : clobber );
  asm volatile("rdtsc" // Esegue RDTSC (il tempo finisce in EDX:EAX)
               : "=a"(val)
               :
               : "edx");

  if (val == 0 || val == 0xFFFFFFFF) {
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
