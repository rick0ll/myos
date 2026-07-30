
#include <kernel/mmap.h>

struct multiboot_tag_mmap *tag_mmap = NULL;
uint32_t get_mmap_entries_num() {
  return (tag_mmap->size - sizeof(struct multiboot_tag_mmap)) /
         tag_mmap->entry_size;
}

void print_mmap() {
  for (int i = get_mmap_entries_num(); i > 0; i--) {
    struct multiboot_mmap_entry *entry = &tag_mmap->entries[i];
    log_info("Type: {x}\n", entry->type);
    log_info("Base addr: {x}\n", entry->base_addr);
    log_info("Length: {x}\n", entry->length);
  }
}
