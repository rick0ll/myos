# Spiegazione

- `#!bin/sh` => Indica all'OS quale programma usare per eseguire il file come script
- `. ./file.sh` => Copia il contento di file.sh nel file attuale
- `set -e` => Se un comando fallisce interrompi (opzione 'errexit')
- `${var1:-var2}` => se var1 è vuoto o non esiste restituisci var2 else var1

## config.sh

**Valorizza le variabili usate nei vari Makefile (Più info in 03_config.md)**

- `grep -Eq -- '-elf($|-)'`:
    - '-E' : attiva regex
    - '-q' : quiet, non mostra a schermo
    - '--' : fine opzioni di grep
    - '-elf' : parola da cercare
    - '(...)' : significa cerca tutto il contenuto dentro () (gruppo di cattura)
    - '$' : fine stringa
    - '|' : or
    - '-' : carattere '-'

## target-to-triple-arch.sh

**Restituisce i386 (nome generale della famiglia di processori intel a 32bit) oppure l'architettura specifica**

- `grep -Eq 'i[[:digit:]]86-`:
    - '[]' : significa cerca un carattere tra quelli dentro [] (classe di cattura)
    - ':' : in qualunque posizione
    - '[:digit:]' : cifra qualsiasi 0-9

- `grep -Eo '^[[:alnum:]_]*`:
    - '-o' : stampa solo i caratteri che matchano, di norma grep ritorna l'intera riga (only matching)
    - '^' : ancoraggio all'inizio, il match deve essere solo all'inizio della frase
    - '[:alnum:]' : cifra qualsiasi + lettera qualsiasi maiuscola e minuscola
    - '\*' : cerca ripetutamente i caratteri dentro [[]]. es abc* => match con abcccc. [[abc]]\* => match anche con abcabcabbc

## clean.sh

**Elimina le cartelle sysroot, isodir. Elimina il file myos.iso e fa make clean di ogni build**

## headers.sh

**Crea la struttura di cartelle (boot, usr/include, usr/lib) con radice sysroot. Copia tutti gli header di ogni libreria (kernel/user) in sysroot**

## build.sh

**Compila ogni file, genera l'archivio (\*.a) contenente ogni eseguibile e genera myos.kernel in isodir/boot**

## iso.sh

**Crea il file di configurazione per GRUB e genera l'immagine ISO dell'OS**

- `i686-elf-grub-mkrescue -o {file_name_output} {source_dir}`:
    - '{source_dir}' : Cartella contenente il file binario del kernel e il file di configurazione di GRUB

_Si usa i686 pk è l'ultimo processore commerciale a 32bit e il progetto è un OS a 32 bit_

```bash
menuentry "hola amigo" {
    multiboot2 /boot/myos.kernel
}
```

- 'menuentry "{entry}"': Definisci una opzione selezionabile nell schermata iniziale di GRUB (Grand Unified Bootloader)
- 'multiboot2' : GRUB passerà dalla modalità di lettura normale alla modalità con le specifiche di multiboot2

## run.sh

**Esegue in ordine i seguenti file:**

    1. clea.sh
    2. headers.sh
    3. build.sh
    4. iso.sh
    5. qemu.sh

## debug.sh

**Avvia l'emulatore in modalità di debug**
