#!/bin/sh

./clean.sh && \
./headers.sh && \
./build.sh && \
./iso.sh

qemu-system-i386 -s -S -cdrom myos.iso
