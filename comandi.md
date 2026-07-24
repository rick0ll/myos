# Freeze mode

qemu-system-i386 -s -S -kernel isodir/boot/myos.kernel

# Check con GDB

x86_64-elf-gdb isodir/boot/myos.kernel

## Comandi GDB

- print/[x,...] [variabile name]
- hb \*[address] => breakpoint
- break [name_function] => ''
- continue => ...
- si => next instruction
- info:
    - registers [register]
- layout:
    - asm
    - regs
