
#include <stdint.h>

uint32_t get_random(void) {
  uint32_t val;

  // Esegue l'istruzione rdrand => num random in base a entropia hw. Se ha
  // successo, restituisce il valore. Altrimenti (raro) fallisce e restituisce
  // 0.
  //
  asm volatile("rdrand %0" : "=r"(val));

  if (val == 0) {
    // siccome kernel per cpu 32bit e clock cpu di 64bit lo divide in due parti
    uint32_t low, high;
    // Legge il contatore dei cicli di clock della CPU
    asm volatile("rdtsc" : "=a"(low), "=d"(high));

    // Mischia i bit con un'operazione XOR (es. usando i cicli di clock bassi)
    // siccome il clock parte sempre da 0 e la funzione viene eseguita subito i
    // valori di low, per quanto variabili, rimangono cmq in un range limitato,
    // con lo xor mischio tutto
    //
    // 0x55A storicamente è il Magic Number che i BIOS x86 cercano alla fine
    // della boot per verificare che sia valido. (Vedi boot.S)
    return low ^ 0x55AA55AA;
  }

  return val;
}
