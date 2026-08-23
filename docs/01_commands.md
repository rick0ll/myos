# Spiegazione

## build.sh

- `sysctl -n hw.logicalcpu`:
    - 'sysctl': (System Control) permette di leggere e modificare parametri del kernel
    - '-n' : (Numeric) di norma viene restituito coppia chiave:valore ora sono valore
    - 'hw.logicalcpu' : variabile specifica con il numero di core logici (core + thread virtuali)

## debug.sh

**Avvia la macchina virtuale con l'OS in modalità di debug**

- `qemu-system-i386 -s -S -cdrom myos.iso`:
    - 'qemu-system-i386' : Avvia l'emulatore QEMU per architettura intel 32-bit
    - '-s' : (Server GDB) avvia anche il server di debug sulla porta 1234
    - '-S' : (Freeze at Startup) non avvia subito esecuzione codice, sarà il debugger a farla partire
    - '-cdrom myos.iso' : inserisce il pagine immagine dell'os, imposta come disco di avvio primario

Apre il GDB (debugger) per x86_64-elf siccome non ho trovato uno per i686-elf, ma funziona uguale, retrocompatibile.

- `x86_64-elf-gdb isodir/boot/myos.kernel -ex "target remote :1234" -ex "b $1`:
    - `-ex target remote :1234` : esegue un comando nel debuger per connetersi alla porta 1234 dove il serve dell'emulatore comunica.
    - `-ex "b $1`: esegue comando per mettere un break point nella funzione voluta.

## qemu.sh

**Avvia la macchina virtuale con l'OS e reindirizza i log a qemu.log**

- `qemu-system-i386 -d cpu_reset -no-reboot -serial file:qemu.log -cdrom myos.iso`:
    - '-d' : (Debug logs) Stampa informazioni di debugger
        - 'cpu_reset' : Stampa informazioni sui registri ogni volta che c'è un reset
    - '-no-reboot': Impedisce il ciclo infinito di riavvii quando il kernel crasha, se triple-faule allora si chiude l'emulatore
    - 'serial' : Reindirizza l'output della porta seriale del PC emulato COM1
        - 'file:qemu.log' : Destinazione del reindirizzamento

## iso.sh

**Crea il file (\*.iso) dell'OS**

`i686-elf-grub-mkrescue -o {file_name_output} {source_dir}`: - '{source_dir}' : Cartella contenente il file binario del kernel e il file di configurazione di GRUB

_Si usa i686 pk è l'ultimo processore commerciale a 32bit e il progetto è un OS a 32 bit_

## debug.sh

**Apre il debugger GDB**

`x86_64-elf-gdb isodir/boot/myos.kernel -ex "target remote :1234"`: Apre il GDB (debugger) per x86_64-elf siccome non ho trovato uno per i686-elf, ma funziona uguale, retrocompatibile. -

- `-ex target remote :1234` : esegue un comando nel debuger per connetersi alla porta 1234 dove il serve dell'emulatore comunica
