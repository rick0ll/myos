#!/bin/sh
set -e
. ./config.sh

mkdir -p "$SYSROOT"

N_CORES=$(expr $(sysctl -n hw.logicalcpu) + 1)

for PROJECT in $SYSTEM_HEADER_PROJECTS; do
  (cd $PROJECT && DESTDIR="$SYSROOT" $MAKE -j "$N_CORES" install-headers)
done
