#!/bin/sh

./clean.sh && \
./headers.sh && \
./build.sh && \
./iso.sh

qemu-system-i386 -s -S -cdrom myos.iso &

# Se non ricevo parametri o se il parametro è vuoto
if [[ $# -eq 0 ]] || [[ -z "$1" ]]; then
    x86_64-elf-gdb isodir/boot/myos.kernel -ex "target remote :1234"
else
    x86_64-elf-gdb isodir/boot/myos.kernel -ex "target remote :1234" -ex "b $1"
fi
