
#include <kernel/gdt.h>
#include <stdint.h>

struct GDT_entry_t GDT_entries[3];
struct GDT_descriptor_t *GDT_descriptor;

void initGDT(void) {
  set_gdt_null_entry();
  set_gdt_kernel_code_entry();
  set_gdt_kernel_data_entry();
  GDT_descriptor->size = (sizeof(GDT_entries) * 3) - 1;
  GDT_descriptor->offset_addr = (uint32_t)GDT_entries;
}

void setGDTentry(int index, uint8_t access, uint8_t flags) {
  GDT_entries[index].access = access;
  GDT_entries[index].granularity = flags | LIMIT_HIGH;
  GDT_entries[index].base_addr_high = BASE_VALUE;
  GDT_entries[index].base_addr_middle = BASE_VALUE;
  GDT_entries[index].base_addr_low = BASE_VALUE;
  GDT_entries[index].segment_limit_low = LIMIT_LOW;
}
