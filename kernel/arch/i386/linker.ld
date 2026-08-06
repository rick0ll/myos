ENTRY(_start)


SECTIONS
{

    /*indirizzo fisico di caricamento di .boot (siccome multiboot header va messo nei primi kb)*/
    . = 1M;

    /*in boot c'è _start, l'entry => siccome prima di tutto all'izinio la paginazione non c'è lable _start
     * non può essere messo in indirizzoi di memoria alti */
    .boot ALIGN(4K) : {
        KEEP(*(.boot))
    }

    kernel_offset  = 0xC0000000;
    /* . += kernel_offset; */
    . += kernel_offset;

    _kernel_start = .;

    /* Unica sezione per l'header e il codice */
    /*metto le sezioni del kernel da 1MB in poi */
    /*AT imposta LMA(Load memory addr), solo riferito al bootloader (multiboot2) che carica il kernel in memoria*/
    /*Il codice compilato ignora AT e pone il codice a 0xC0100000 oltre*/
    /*Le label non vengono spostate con AT, nemmeno quelle in boot.s pk sono valorizzate (sostituite con num) dal linker, prima di bootloading*/
    .text ALIGN(4K) : AT(ADDR(.text) - kernel_offset)
    {

        /* 2. Il codice sorgente subito dopo */
        *(.text*)
    }

    .rodata ALIGN(4K) : AT(ADDR(.rodata) - kernel_offset)
    {
        *(.rodata*)
    }

    .data ALIGN(4K) : AT(ADDR(.data) - kernel_offset)
    {
        *(.data*)
    }

    .bss ALIGN(4K) : AT(ADDR(.bss) - kernel_offset)
    {
        *(COMMON)
        *(.bss*)
    }

    _kernel_end = .;
}
