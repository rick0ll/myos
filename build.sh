#!/bin/sh
#set -e := se qlcs fallisce blocca tutto, non continuare
set -e
. ./headers.sh

N_CORES=$(expr $(sysctl -n hw.logicalcpu) + 1)

for PROJECT in $PROJECTS; do
    (cd $PROJECT && DESTDIR="$SYSROOT" $MAKE -j "$N_CORES" install)
done
