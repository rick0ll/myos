/*
 * ============================================================================
 * Questo file viene usato come supporto nella creazione dell'header corretto
 * per multiboot2 e per la elaborazione delle informazioni trasmesse dal
 * bootloader.
 * ============================================================================
 * */

#ifndef MULTIBOOT_HEADER
#define MULTIBOOT_HEADER 1

#define ALIGNMENT 8

// Si tratta di un valore 'magico' utilizzato per verificare che si abbia
// veramente richiesto il tipo di bootloader, multiboot2
#define MAGIC_HEADER_MULTIBOOT2_NUMBER 0xE85250D6

// Si tratta di un valore 'magico' che multiboot2 passa nel registro %eax che
// conferma che multiboot2 è stato usato
#define MAGIC_MULTIBOOT2_PROOF_NUMBER 0x36D76289

// Valore che indica al bootloader l'architettura target
#define ARCH_I386_PROTECTED 0

/* ------------------------------------------------------------------------
 * Le seguenti costanti servono come supporto per la richiesta di azioni
 * aggiuntive a multiboot2  */

#define OPTIONAL 1
#define MANDATORY 0

// Tag per definire il punto di entrata del codice assembly
#define ENTRY_POINT_TAG 3

// Tag per abilitare lo schermo in modalità framebuffer (non VGA) e con
// determinate dimensioni
#define FRAMEBUFFER_TAG 5
#define WIDTH 2048
#define HEIGHT 1024
#define COLORS_PER_PIXEL 32

// Tag per imporre l'allineamento dei moduli (file caricati prima del kernel in
// RAM) a 4KB
#define MODULE_ALIGNMENT_TAG 6

// Tag per abilitare il rilocamento del codice del kernel in una qualsiasi
// posizione tra start_position ed end_position
#define RELOCATION_KERNEL_TAG 10
#define KERNEL_START_POSITION 0x00100000
#define KERNEL_END_POSITION 0x0FFFFFFF
#define KERNEL_PAGING_ALIGNMENT 4096
#define POSITION_PREFERENCE_NONE 0
#define POSITION_PREFERENCE_LOW 1
#define POSITION_PREFERENCE_HIGH 2

/*
 * Costanti di supporto per la richiesta di informazioni aggiuntive a
 * multiboot2
 * */

// Tag per richiedere informazioni aggiuntive al bootloader
#define INFORMATION_REQUEST_TAG 1

// Ricevo la stringa aggiutiva passata nel comando di avvio di qemu (come uno
// strv)
#define MULTIBOOT_COMMAND_LINE_INFO 1

// Ricevo il nome del bootloader (multiboot2 in questo caso)
#define MULTIBOOT_BOOTLOADER_NAME_INFO 2

// Ricevo informazioni su dove si trovano i moduli (file caricati prima del
// kernel) in memoria
#define MULTIBOOT_MODULES_RAM_ADDRESSES_INFO 3

// Ricevo informazioni di base sulla memoria come quanta ce n'è senza
// distinguere se libera o meno
#define MULTIBOOT_BASIC_MEMORY_INFO 4

// Ricevo informazioni dettagliate su come è disposta la memoria e che tipo di
// memoria si tratta
#define MULTIBOOT_MEMORY_MAP_INFO 6

// Ricevo informazioni necessarie per gestire lo schermo in modalità framebuffer
#define MULTIBOOT_FRAMEBUFFER_INFO 8

// Ricevo la tabella delle sezioni ELF
#define MULTIBOOT_ELF_SYMBOLS_INFO 9

// Ricevo la tabella SMBIOS
#define MULTIBOOT_SMBIOS_TABLE_INFO 13

// Ricevo la tabella ACPI (v1 o v2)
#define MULTIBOOT_ACPI_TABLE_V1_INFO 14
#define MULTIBOOT_ACPI_TABLE_V2_INFO 15

// Ricevo informazioni della scheda rete (solo se il kernel viene avviato via
// rete)
#define MULTIBOOT_NETWORKING_INFO 16

/*
 * Fine della sezione di richiesta informazioni al bootloader
 * ------------------------------------------------------------
 */

// Tag per indicare la fine della serie tag
#define END_REQUEST_TAG 0

// Stack di dimensioni 16KB
#define STACK_SIZE 16384

// Ogni entry della stack deve essere allineato a 16
#define STACK_ALIGNMENT 16

#ifndef __ASSEMBLER__

#include <stdint.h>
/* =========================================================================
 * ESTRUTTURE DI BASE MBI (MULTIBOOT INFORMATION)
 * ========================================================================= */

// Intestazione generale di OGNI Tag MBI (8 byte)
struct multiboot_tag {
  uint32_t type;
  uint32_t size;
} __attribute__((packed));

// Intestazione dell'intero blocco MBI (passato in EBX)
struct multiboot_info_header {
  uint32_t total_size;
  uint32_t reserved;
} __attribute__((packed));

/* =========================================================================
 * STRUTTURE SPECIFICHE DEI TAG MBI
 * ========================================================================= */

// MBI Tag 1: Boot Command Line
struct multiboot_tag_string {
  uint32_t type; // 1
  uint32_t size;
  char string[]; // Stringa C con \0 finale
} __attribute__((packed));

// MBI Tag 2: Bootloader name
struct multiboot_tag_bootloader_name {
  uint32_t type; // 1
  uint32_t size;
  char string[]; // Stringa C con \0 finale
} __attribute__((packed));

// MBI Tag 3: Modules
struct multiboot_tag_module {
  uint32_t type; // 3
  uint32_t size;
  uint32_t mod_start; // Indirizzo FISICO di inizio
  uint32_t mod_end;   // Indirizzo FISICO di fine
  char string[];      // Nome/parametri del modulo
} __attribute__((packed));

// MBI Tag 4: Basic Memory Info
struct multiboot_tag_basic_memory_info {
  uint32_t type; // 4
  uint32_t size;
  uint32_t mem_start;
  uint32_t mem_end;
} __attribute__((packed));

// Singola voce della Memory Map (24 byte)
struct multiboot_mmap_entry {
  uint64_t base_addr; // Valore intero a 64 bit!
  uint64_t length;    // Valore intero a 64 bit!
  uint32_t type;
#define MULTIBOOT_MEMORY_AVAILABLE 1
#define MULTIBOOT_MEMORY_RESERVED 2
#define MULTIBOOT_MEMORY_ACPI_RECLAIMABLE 3
#define MULTIBOOT_MEMORY_NVS 4
#define MULTIBOOT_MEMORY_BADRAM 5
  uint32_t reserved;
} __attribute__((packed));

// MBI Tag 6: Memory Map
struct multiboot_tag_mmap {
  uint32_t type; // 6
  uint32_t size;
  uint32_t entry_size;    // Di solito 24 byte
  uint32_t entry_version; // 0
  struct multiboot_mmap_entry entries[];
} __attribute__((packed));

// MBI Tag 9: ELF Symbols
struct multiboot_tag_elf_sections {
  uint32_t type; // 9
  uint32_t size;
  uint16_t num;
  uint16_t entsize;
  uint16_t shndx;
  uint16_t reserved;
  uint8_t section_headers[]; // Dati grezzi delle sezioni ELF
} __attribute__((packed));

// MBI Tag 8: Framebuffer Info
struct multiboot_tag_framebuffer {
  uint32_t type;
  uint32_t size;
  uint64_t framebuffer_addr;   // Indirizzo della RAM video (intero 64 bit)
  uint32_t framebuffer_pitch;  // Byte per riga
  uint32_t framebuffer_width;  // Larghezza in px
  uint32_t framebuffer_height; // Altezza in px
  uint8_t framebuffer_bpp;     // Bit per pixel (es. 32)
  uint8_t framebuffer_type;    // 0 = Indexed, 1 = RGB, 2 = EGA Text
  uint8_t reserved;

  // I dettagli del colore seguono qui in RAM a seconda di framebuffer_type
} __attribute__((packed));

// MBI Tag 13: SMBIOS Tables
struct multiboot_tag_smbios {
  uint32_t type; // 13
  uint32_t size;
  uint8_t major;
  uint8_t minor;
  uint8_t reserved[6];
  uint8_t tables[]; // Dati grezzi SMBIOS
} __attribute__((packed));

// Struttura ACPI RSDP v1 (20 byte)
struct acpi_rsdp_v1 {
  char signature[8]; // "RSD PTR "
  uint8_t checksum;
  char oem_id[6];
  uint8_t revision;      // 0 per v1.0
  uint32_t rsdt_address; // Indirizzo a 32 bit della tabella RSDT
} __attribute__((packed));

// Struttura ACPI RSDP v2 (36 byte)
struct acpi_rsdp_v2 {
  struct acpi_rsdp_v1 v1;
  uint32_t length;
  uint64_t xsdt_address; // Indirizzo a 64 bit della tabella XSDT
  uint8_t extended_checksum;
  uint8_t reserved[3];
} __attribute__((packed));

// MBI Tag 14: ACPI v1 RSDP
struct multiboot_tag_acpi_old {
  uint32_t type; // 14
  uint32_t size;
  struct acpi_rsdp_v1 rsdp; // Copia diretta dell'RSDP v1
} __attribute__((packed));

// MBI Tag 15: ACPI v2+ RSDP
struct multiboot_tag_acpi_new {
  uint32_t type; // 15
  uint32_t size;
  struct acpi_rsdp_v2 rsdp; // Copia diretta dell'RSDP v2
} __attribute__((packed));

// MBI Tag 16: Networking Information
struct multiboot_tag_network {
  uint32_t type; // 16
  uint32_t size;
  uint8_t dhcp_ack[]; // Dati del pacchetto DHCP
} __attribute__((packed));

#endif /* MULTIBOOT_HEADER */
#endif
