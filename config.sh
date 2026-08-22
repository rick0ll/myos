#
# ========================================================================
# FILE DI CONFIGURAZIONE USATO PER LA COMPILAZIONE CORRETTA DEL PROGETTO
# ========================================================================
#

# SYSTEM_HEADER_PROJECTS contiene i nomi delle cartelle con i file da compilare seperati da uno spazio
SYSTEM_HEADER_PROJECTS="libc kernel"

# PROJECTS contiene i nomi delle cartelle con i file da compilare seperati da uno spazio
PROJECTS="libc kernel"

# TARGET è il nome dell'architettura su cui si baserà l'OS
export TARGET=${TARGET:-"-i686-elf"}

export MAKE=${MAKE:-make}
export HOST=${HOST:-$(./default-host.sh $TARGET)}

# Archiver
export AR=${HOST}-ar

# Assembler
export AS=${HOST}-as

# Cross-Compiler
export CC=${HOST}-gcc

# --------------------------------------
# Cartelle dell'OS
#
# Radice di cartelle principale per programmi e file utente
export PREFIX=/usr

# Radice di cartelle per i file eseguibili
export EXEC_PREFIX=$PREFIX

# Cartella dove saranno salvati i file di avvio (kernel.bin, myos.iso)
export BOOTDIR=/boot

# Cartella dove verranno copiate le librerie (*.a)
export LIBDIR=$EXEC_PREFIX/lib

# Cartella dove verranno copiati i file intestazione
export INCLUDEDIR=$PREFIX/include
# ---------------------------------------

# Flag di compilazione
export CFLAGS='-O2 -g -fstack-protector-strong -mno-tls-direct-seg-refs -fno-pic'
export CPPFLAGS='-g -fstack-protector-strong -mno-tls-direct-seg-refs -fno-pic'

# Cartella radice dell'OS
export SYSROOT="$(pwd)/sysroot"
export CC="$CC --sysroot=$SYSROOT"

# Se $HOST contiene '-elf' significa che è il compilatore è di tipo '-elf' (bare metal) =>
# non cercherà una cartella /include con header come stdio.h, string.h, stdlib.h etc
if echo "$HOST" | grep -Eq -- '-elf($|-)'; then

    # Aggiungo a CC l'indirizzo della cartella con i file di intestazione
  export CC="$CC -isystem=$INCLUDEDIR"
fi
