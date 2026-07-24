#!/bin/sh
set -e
. ./iso.sh

# qemu-system-$(./target-triplet-to-arch.sh $HOST) -d cpu_reset -no-reboot -serial file:qemu.log \

qemu-system-i386 -d cpu_reset -no-reboot -serial file:qemu.log \
-cdrom myos.iso
