#!/bin/sh
set -e
. ./config.sh

N_CORES=$(expr $(sysctl -n hw.logicalcpu) + 1)

for PROJECT in $PROJECTS; do
  (cd $PROJECT && $MAKE -j "$N_CORES" clean)
done

rm -rf sysroot
rm -rf isodir
rm -rf myos.iso
