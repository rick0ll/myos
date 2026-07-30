#!/bin/sh
set -e
. ./build.sh

mkdir -p isodir
mkdir -p isodir/boot
mkdir -p isodir/boot/grub

cp sysroot/boot/myos.kernel isodir/boot/myos.kernel
#Questo è un trucco della shell per scrivere un file di testo al volo. Crea il file di configurazione di GRUB (grub.cfg).
#Quando il PC si avvia, GRUB leggerà questo file e mostrerà la schermata di selezione con la voce "myos",
#sapendo che per avviarlo deve caricare il file /boot/myos.kernel usando lo standard multiboot.
cat > isodir/boot/grub/grub.cfg << EOF
menuentry "hola amigo" {
	multiboot2 /boot/myos.kernel
}
EOF
i686-elf-grub-mkrescue -o myos.iso isodir
