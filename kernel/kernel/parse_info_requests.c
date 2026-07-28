
#include <kernel/parse_info_requests.h>

void parse_info_request(uintptr_t addr) {
  if (addr & 7) {
    log_error("Multiboot2: Indirizzo non allineato a 8-byte: {x}\n", addr);
    return;
  }

  struct multiboot_tag *tag = (struct multiboot_tag *)(addr + 8);
  while (tag->type != END_REQUEST_TAG) {
    uint32_t size = tag->size;

    switch (tag->type) {
    case MULTIBOOT_COMMAND_LINE_INFO:
      struct multiboot_tag_string *tag_string =
          (struct multiboot_tag_string *)tag;
      /* log_info("{s}\n", tag_string->string); */
      break;

    case MULTIBOOT_MODULES_RAM_ADDRESSES_INFO:
      struct multiboot_tag_module *tag_module =
          (struct multiboot_tag_module *)tag;
      /* log_info("MULTIBOOT MODULE RAM ACCESS INFO\n"); */
      /* log_info("Nomi: {s}\n", tag_module->string); */
      /* log_info("Mod End addr: {x}\n", tag_module->mod_end); */
      /* log_info("Mod Start addr: {x}\n", tag_module->mod_start); */
      break;

    case MULTIBOOT_MEMORY_MAP_INFO:
      /* log_info("Entries Over\n\n"); */
      extern struct multiboot_tag_mmap *tag_mmap;
      tag_mmap = (struct multiboot_tag_mmap *)tag;
      break;
    case MULTIBOOT_BOOTLOADER_NAME_INFO:
      struct multiboot_tag_bootloader_name *tag_bootloader_name =
          (struct multiboot_tag_bootloader_name *)tag;
      break;
    case MULTIBOOT_BASIC_MEMORY_INFO:
      struct multiboot_tag_basic_memory_info *tag_basic_memory_info =
          (struct multiboot_tag_basic_memory_info *)tag;
      break;
    case MULTIBOOT_ACPI_TABLE_V1_INFO:
      struct multiboot_tag_acpi_old *tag_acpi_old =
          (struct multiboot_tag_acpi_old *)tag;
      struct acpi_rsdp_v1 *tag_rsdp_1 = &tag_acpi_old->rsdp;
      break;
    case MULTIBOOT_ACPI_TABLE_V2_INFO:
      struct multiboot_tag_acpi_new *tag_acpi_new =
          (struct multiboot_tag_acpi_new *)tag;
      struct acpi_rsdp_v2 *tag_rsdp_2 = &tag_acpi_new->rsdp;
      break;

    case MULTIBOOT_ELF_SYMBOLS_INFO:
      struct multiboot_tag_elf_sections *tag_elf_sections =
          (struct multiboot_tag_elf_sections *)tag;
      break;
    case MULTIBOOT_NETWORKING_INFO:
      struct multiboot_tag_network *tag_network =
          (struct multiboot_tag_network *)tag;
      uint32_t len = tag_network->size / sizeof(size);
      break;
    case MULTIBOOT_FRAMEBUFFER_INFO:
      struct multiboot_tag_framebuffer *tag_framebuffer =
          (struct multiboot_tag_framebuffer *)tag;
    case MULTIBOOT_SMBIOS_TABLE_INFO:
      struct multiboot_tag_smbios *tag_smbios =
          (struct multiboot_tag_smbios *)tag;
    default:
      /* log_warn("Multiboot tag di richiesta non riconosciuto, ignoro\n"); */
      break;
    }

    // rendo uniptr pk sommano a un ptr il compile fa ptr + (num *
    // sizeof(tipo del puntato di ptr)), unitptr è solo una variabile dunque
    // non succedde
    uintptr_t next_addr = (uintptr_t)tag + size;
    // Per essere allineato a 8byte multiboot2 può creare un padding tra i tag
    // che va gestito
    next_addr = (next_addr + 7) & ~7;
    tag = (struct multiboot_tag *)next_addr;
  }
}
