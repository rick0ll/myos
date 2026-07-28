
#include <kernel/mmap.h>

struct multiboot_tag_mmap *tag_mmap = NULL;
uint32_t get_mmap_entries_num() {
  return (tag_mmap->size - sizeof(struct multiboot_tag_mmap)) /
         tag_mmap->entry_size;
}
