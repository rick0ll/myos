

#include <stdint.h>
#include <stdlib.h>

int dec_to_hex(uint32_t num, unsigned char *hex) {
  const char hex_chars[] = "0123456789ABCDEF";

  hex[0] = '0';
  hex[1] = 'x';

  if (num == 0) {
    hex[2] = '0';
    hex[3] = '\0';
    return 4;
  }

  char buff[16];
  int pos = 0;
  while (num > 0) {
    // compiler traduce da solo a 0000000F
    buff[pos++] = hex_chars[num & 0x0F]; // Prende gli ultimi 4 bit =>
                                         // & con 00000000...00001111
    num >>= 4; // Shift a destra di 4 bit (equivalente a num /= 16)
  }

  int dest_pos = 2;
  while (pos > 0) {
    hex[dest_pos++] = buff[--pos];
  }

  hex[dest_pos] = '\0';
  return dest_pos;
}
