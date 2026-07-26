// # Declare constants for the multiboot header.
// .set .align,    1<<0             # align loaded modules on page boundaries
// .set MEMINFO,  1<<1             # provide memory map
// .set FLAGS,    .align | MEMINFO  # this is the Multiboot 'flag' field
// .set MAGIC,    MANDATORYxE85250D6       # 'magic number' lets bootloader find the header
// .set CHECKSUM, -(MAGIC + FLAGS) # checksum of above, to prove we are multiboot

#define ASM_FILE 1
#include <multiboot2.h>


.set HEADER_LENGTH, multiboot_header_end - multiboot_header
.set CHECKSUM, -( MAGIC_HEADER_MULTIBOOT2_NUMBER + ARCH_I386_PROTECTED + HEADER_LENGTH)

.section .boot
.align ALIGNMENT

multiboot_header:
    .long MAGIC_HEADER_MULTIBOOT2_NUMBER
    .long ARCH_I386_PROTECTED
    .long HEADER_LENGTH
    .long CHECKSUM

    //.align forza allineamento dati in multipli di 8 byte

    //tag format
    // type
    // obbligatorio (0) opzionale (1)
    // size
    //DEFINISCO ENTRY POINT OS
    .align ALIGNMENT
    entry_point_tag_start:
        .short ENTRY_POINT_TAG
        .short MANDATORY
        .long entry_point_tag_end - entry_point_tag_start
        .long kernel_start
    entry_point_tag_end:

    //DEFINISCO ASPETTO E DIMENSIONI FINESTRA
    // TODO
    //molto più complesso di VGA, da fare dopo
    /*
    .align ALIGNMENT
    framebuffer_tag_start:
        .short FRAMEBUFFER_TAG
        .short OPTIONAL
        .long framebuffer_tag_end - framebuffer_tag_start
        .long WIDTH
        .long HEIGHT
        .long COLORS_PER_PIXEL
    framebuffer_tag_end:
    */

    //DICO CHE LA MEMORIA IN RAM DEI MODULI DEVE ESSERE ALLINEATA A 4KB (MODULI SONO FILE CARICATI IN RAM PRIMA DEL KERNEL)
    .align ALIGNMENT
     module_alignment_start:
        .short MODULE_ALIGNMENT_TAG
        .short OPTIONAL
        .long module_alignment_end - module_alignment_start
    module_alignment_end:

    //DEFINISCO RANGE POSZIONE IN MEMORIA DEL KERNEL
    .align ALIGNMENT
    relocatable_kernel_start:
        .short RELOCATION_KERNEL_TAG
        .short OPTIONAL
        .long relocatable_kernel_end - relocatable_kernel_start
        .long KERNEL_START_POSITION
        .long KERNEL_END_POSITION
        .long KERNEL_PAGING_ALIGNMENT
        .long POSITION_PREFERENCE_LOW
    relocatable_kernel_end:

    //DICO CHE VOGLIO INFORMAZIONI AGGIUTIVE DAL BOOTLOADER
    .align ALIGNMENT
    information_request_start:
        .short INFORMATION_REQUEST_TAG
        .short OPTIONAL
        .long information_request_end - information_request_start

        .long MULTIBOOT_COMMAND_LINE_INFO  //dice quali opz avvio kernel
        .long MULTIBOOT_MODULES_RAM_ADDRESSES_INFO //dice dove si trovano i moduli nella RAM libera
        .long MULTIBOOT_MEMORY_MAP_INFO //chiede info Memory Map => mappa completa RAM libera da usare
        .long MULTIBOOT_ELF_SYMBOLS_INFO//Da la tabellbba simboli degli eseguibili
        .long MULTIBOOT_FRAMEBUFFER_INFO
        .long MULTIBOOT_SMBIOS_TABLE_INFO
        .long MULTIBOOT_ACPI_TABLE_V1_INFO
        .long MULTIBOOT_ACPI_TABLE_V2_INFO  //Da puntatore a tabelle ACPI: si può vedere dal kernel num. core e CPU e altro
        .long MULTIBOOT_NETWORKING_INFO  //Info su scheda di rete
    information_request_end:

    //DICO CHE HO FINITO I TAG
    .align ALIGNMENT
    final_tag_start:
        .short END_REQUEST_TAG
        .short MANDATORY
        .long final_tag_end - final_tag_start
    final_tag_end:
multiboot_header_end:

# Reserve a stack for the initial thread.
.section .bss
.align STACK_ALIGNMENT
stack_bottom:
    .skip STACK_SIZE
stack_top:

# The kernel entry p.long.
.section .text
.global kernel_start
.type kernel_start, @function
kernel_start:

    cmpl $0x36D76289, %eax//se bootloader è multiboot2 allora in eax c'è il valore else non è il bootloader che il mio kernel usa
    jne .bad_bootloader

    //preparare %esp per avere stack p.longer pronta per l'uso della stack
    movl $stack_top, %esp



    //tolgo 16 / ultimi 4 bit da %esp per fare che sia un multiplo di 16 che CPU richiede per qualche motivo
    //-16 pk è 16 (000.0010000) invece -16 è (1111...110000)
    andl $-16, %esp

    //pushl è per 32bit data push per 64bit
    pushl %ebx // pusho su stack per il mio kernel in C pk grub ha messo le info che ho richieste nella memoria puntata dal registro ebx

    # Call the global constructors.
//    call _init

    # Transfer control to the main kernel.
    call kernel_main

    # Hang if kernel_main unexpectedly returns.
    cli
1:	hlt
    jmp 1b

.size kernel_start, . - kernel_start

.global .bad_bootloader

.bad_bootloader:
    movw $0x2F41, (0xB8000)
    /* Disattiva gli interrupt ed entra in loop infinito per arrestare la CPU qui */
    cli
    hlt
    jmp .bad_bootloader
