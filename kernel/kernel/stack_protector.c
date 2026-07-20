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
