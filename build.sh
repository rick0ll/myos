#!/bin/sh

set -e

. ./headers.sh

# Core + 1, pk se un core si ferma, idle, ha qlcs da fare
N_CORES=$(expr $(sysctl -n hw.logicalcpu) + 1)


for PROJECT in $PROJECTS; do
    (cd $PROJECT && DESTDIR="$SYSROOT" $MAKE -j "$N_CORES" install)
done
