#include <multiboot2.h>
#include <kernel/gdt.h>
#include <kernel/vmm.h>
#include <kernel/memory.h>


.set HEADER_LENGTH, multiboot_header_end - multiboot_header
.set CHECKSUM, -(MAGIC_HEADER_MULTIBOOT2_NUMBER + ARCH_I386_PROTECTED + HEADER_LENGTH)

.section .boot, "ax"
.align ALIGNMENT

multiboot_header:
    .long MAGIC_HEADER_MULTIBOOT2_NUMBER
    .long ARCH_I386_PROTECTED
    .long HEADER_LENGTH
    .long CHECKSUM


    //DEFINISCO ENTRY POINT OS
    .align ALIGNMENT
    entry_point_tag_start:
        .short ENTRY_POINT_TAG
        .short MANDATORY
        .long entry_point_tag_end - entry_point_tag_start
        .long _start
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
    /*
    ; .align ALIGNMENT
    ; relocatable_kernel_start:
    ;     .short RELOCATION_KERNEL_TAG
    ;     .short OPTIONAL
    ;     .long relocatable_kernel_end - relocatable_kernel_start
    ;     .long KERNEL_START_POSITION
    ;     .long KERNEL_END_POSITION
    ;     .long KERNEL_PAGING_ALIGNMENT
    ;     .long POSITION_PREFERENCE_LOW
    ; relocatable_kernel_end:
    */

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

.global _start
_start:

    cmpl $MAGIC_MULTIBOOT2_PROOF_NUMBER, %eax//se bootloader è multiboot2 allora in eax c'è il valore else non è il bootloader che il mio kernel usa
    jne bad_bootloader

       //preparare %esp per avere stack p.longer pronta per l'uso della stack
    movl $(stack_top - KERNEL_VIRT_BASE), %esp

    //tolgo 16 / ultimi 4 bit da %esp per fare che sia un multiplo di 16 che CPU richiede per qualche motivo
    //-16 pk è 16 (000.0010000) invece -16 è (1111...110000)
    andl $-16, %esp

    //pushl è per 32bit data push per 64bit
    add KERNEL_VIRT_BASE, %ebx
    pushl %ebx
    //ebx ha il indirizzo che multiboot ha passato con le info, lo salvo nella stack

    // 1. Carichiamo la Page Directory
    //la label non è stata spostata da AT del linker ma i dati si, sono stati spostati da GRUB
    //dunque sposto dove punta label
    mov $(boot_page_directory - KERNEL_VIRT_BASE), %ebx

    mov %ebx, %cr3


    //per modificae registri controllo crx bisogna sempre passare per reg
    //2. attivohe pages 4mb per sicurezz dimensioni kernel
    mov %cr4, %ecx
    or $0x00000010, %ecx
    mov %ecx, %cr4

    //3. attivo paginazione
    mov %cr0, %ecx
    or $0x80000000, %ecx
    mov %ecx, %cr0

 //jmp bad_bootloader
    //le label non sono modificate da AT in linker.ld dunque higher half è già a 0xC000000 oltre
    //uso lea (load effective addr) per prendre addr assoluto e non solo un offset
    // lea higher_half, %ecx
    // jmp *%ecx


    //jmp bad_bootloader
    jmp higher_half

.size _start, . - _start
# Reserve a stack for the initial thread.
.section .bss
.align STACK_ALIGNMENT
stack_bottom:
    .skip STACK_SIZE
stack_top:

.section .data
.align 8
gdt_start:
    .quad  GDT_ENTRY_NULL               // 0x00: Null Descriptor
    .quad  GDT_ENTRY_KERNEL_CODE    // 0x08: Kernel Code Segment (0x0 - 4GB, Ring 0)
    .quad  GDT_ENTRY_KERNEL_DATA    // 0x10: Kernel Data Segment (0x0 - 4GB, Ring 0)
gdt_end:

gdt_descriptor:
.short gdt_end - gdt_start - 1 #Limit => num ultimo elemento
.int gdt_start                  #gdt addr

.align 4096

boot_page_directory:
    # PDE 0: Identity Map (0x00000000 -> 0x00000000) con bit Present, RW, Page Size (4MB)
    //questa è la prima entry di pde, con indirizzo  Page table 0x0
    //punta a addr phys = 0x0 - 4MB dove c'è il codice fisico del booting (boot.s)
    //booting < 4MB
    .long KERNEL_PAGE_BOOT
    //da  entry pde 1 fino a 767 posti a 0 => arrivo fino a 3GB in pde index
    .fill 767, 4, 0
    # PDE 768: Higher Half (KERNEL_VIRT_BASE -> 0x00000000)
    //la entry punta allo stesso addr fisico di PDE[0] ma cambia l'indirizzo virtuale pk è in pos 768 => 768 pagine * 4MB/pagina = 3GB
    .long KERNEL_PAGE_BOOT
    //Restanti 255 dei 1024 entries
    .fill 255, 4, 0


.section .text
higher_half:

    //sistemo esp
    add $KERNEL_VIRT_BASE, %esp

    //tolgo pde[0]
    //movl 32bit
    movl $0, boot_page_directory

    //invalido cache
    // mov %cr3, %ecx
    // mov %ecx, %cr3

    //invalido pde[0]
    invlpg (0)


    lgdt (gdt_descriptor)

    mov $KERNEL_DATA_SELECTOR, %ax //0x10 => 2 * 8Byte => offset dove trovare gdt entry
    mov %ax, %ds
    mov %ax, %es
    mov %ax, %fs
    mov %ax, %gs
    mov %ax, %ss


    ljmp $KERNEL_CODE_SELECTOR, $ip_set //0x08 => 1 * 8Byte

ip_set:
    // Call the global constructors.
    call _init

    //Transfer control to the main kernel.
    call kernel_main
    jmp hang

bad_bootloader:
    movl $0x000B8000, %edi
    movw $0x2F41, (%edi)
    /* Disattiva gli interrupt ed entra in loop infinito per arrestare la CPU qui */
    cli
    hlt
    jmp bad_bootloader


hang:
    # Hang if kernel_main unexpectedly returns.
    cli
1:	hlt
    jmp 1b



