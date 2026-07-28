ENTRY(kernel_entry)

SECTIONS
{
    . = 1M;

    _kernel_start = .;
    /* Unica sezione per l'header e il codice */
    .text : ALIGN(4K)
    {
        /* 1. L'header Multiboot2 DEVE stare nei primissimi byte */
        . = ALIGN(8);
        KEEP(*(.boot))

        /* 2. Il codice sorgente subito dopo */
        *(.text*)
    }

    .rodata : ALIGN(4K)
    {
        *(.rodata*)
    }

    .data : ALIGN(4K)
    {
        *(.data*)
    }

    .bss : ALIGN(4K)
    {
        *(COMMON)
        *(.bss*)
    }

    _kernel_end = .;
}
