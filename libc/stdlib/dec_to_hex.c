
#include <stdint.h>
#include <stdlib.h>

void dec_to_hex(uint32_t num, char hex[11]) {
  const char hex_chars[] = "0123456789ABCDEF";

  // Un intero a 32-bit ha ESATTAMENTE 8 cifre esadecimali (4 bit ciascuna)
  // Riempiamo l'array da DESTRA verso SINISTRA per evitare di dovere fare lo
  // SWAP!

  hex[0] = '0';
  hex[1] = 'x';
  for (int i = 9; i >= 2; i--) {
    hex[i] = hex_chars[num & 0xF]; // Prende gli ultimi 4 bit =>
                                   // & con 00000000...00001111
    num >>= 4; // Shift a destra di 4 bit (equivalente a num /= 16)
  }

  hex[10] = '\0'; // Terminatore a posizione 8 (l'array deve essere grande
                  // almeno 9 byte)
}
