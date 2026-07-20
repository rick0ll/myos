#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

#include <kernel/tty.h>

#include "vga.h"

static const size_t vga_width = 80;
static const size_t vga_height = 25;
static uint16_t *const vga_memory = (uint16_t *)0xb8000;

static size_t terminal_row;
static size_t terminal_column;
static uint8_t terminal_color;
static uint16_t *terminal_buffer;

void terminal_initialize(void) {
  terminal_row = 0;
  terminal_column = 0;
  terminal_color = vga_entry_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);
  terminal_buffer = vga_memory;
  for (size_t y = 0; y < vga_height; y++) {
    for (size_t x = 0; x < vga_width; x++) {
      const size_t index = y * vga_width + x;
      terminal_buffer[index] = vga_entry(' ', terminal_color);
    }
  }
}

void terminal_setcolor(uint8_t color) { terminal_color = color; }

void terminal_putentryat(unsigned char c, uint8_t color, size_t x, size_t y) {
  const size_t index = y * vga_width + x;
  terminal_buffer[index] = vga_entry(c, color);
}

void terminal_scroll() {

  for (size_t y = 0; y < (vga_height - 1); y++) {
    for (size_t x = 0; x < vga_width; x++) {
      int index = y * vga_width + x;
      int index_next_row = (y + 1) * vga_width + x;
      terminal_buffer[index] = terminal_buffer[index_next_row];
    }
  }
}

void terminal_delete_last_line() {
  for (size_t x = 0; x < vga_width; x++) {
    int index = (vga_height - 1) * vga_width + x;
    terminal_buffer[index] = vga_entry(' ', terminal_color);
  }
}

void terminal_putchar(char c) {
  unsigned char uc = c;

  if (c == '\n') {
    terminal_column = 0;
    if (++terminal_row == vga_height) {
      terminal_scroll();
      terminal_delete_last_line();
      terminal_row = vga_height - 1;
    }
    return;
  }

  terminal_putentryat(uc, terminal_color, terminal_column, terminal_row);
  if (++terminal_column == vga_width) {
    terminal_column = 0;
    if (++terminal_row == vga_height) {
      terminal_scroll();
      terminal_delete_last_line();
      terminal_row = vga_height - 1;
    }
  }
}

void terminal_write(const char *data, size_t size) {
  for (size_t i = 0; i < size; i++)
    terminal_putchar(data[i]);
}

void terminal_writestring(const char *data) {
  terminal_write(data, strlen(data));
}
