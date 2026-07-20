#!/bin/sh
set -e
. ./iso.sh

qemu-system-$(./target-triplet-to-arch.sh $HOST) -d cpu_reset -no-reboot -cdrom myos.iso
