# Spiegazione

## Configurazione del cross-compiler

- 'Target i686-elf' : Compila per un'architettura x86 a 32-bit in formato generico elf (formato bare-metal)

### Flags

- `--disable-nls` : (National Language Support) Ouptut compiler (warning, errori) in inglese solamente
- `--without-headers` : Non include gli header del PC host e non configurare i percorsi automatici della sysroot
- `--enable-languages=...` : Compilatore per i linguaggi dopo '='
- `--with-system-zlib` : Per compilare viene usata la libreria di compressione di sistema (zlib) così da ridurre le dimensioni del file

## VARIABILI

- `TARGET` : (Target Triplet) E' una stringa standard con tre elementi divisi da '-':
    - '\[Architettura]' : (i686) Serve per specificare il set di istruzioni hardware che la CPU deve saper eseguire, il compiler usa questa informazione per
      tradurre correttamente il codice C in ASM

    - '\[Produttore]' : () Indica il produttore dell'architettura (hw), se non c'è, nel caso di PC generico basta saltarlo

    - '\[Sistema Operativo]' : (elf) Indica al compiler a quale sistema operativo fare affidamento (linux/windows/...) per cose come system-call, punto di ingresso del programma.
      Se c'è scritto 'elf' vuol dire bare-metal, nessun OS

- `AR` : (Archive) Gli archivi vengono usati per rappresentare le librerie statiche ossia le raccolte di file oggetto (\*.o) pronti per essere
  collegati a un programma tramite il linker.

- `AS` : (Assembler) Trasforma codice asm in binario in un file oggetto (\*.o)

- `CC` : (Compiler) Trasforma codice in asm:
    - '--sysroot' : Indica la cartella root nella quale troverà le intestazioni (\*.h), e librerie (\*.a) del sistema operativo (radice dell'OS)
      Se il compilatore è configurato con opzione --without-headers (caso mio) sysroot si limita a dire al compiler quale sarà la cartella delle librerie, ignora intestazioni

    - 'isystem' : Indica la cartella degli header

_Parliamo sempre di header e archivi di librerie dell'OS (stdio.h, string.h, ...)
le librerie Freestanding (stdint.h, stdbool.h, ...) ci sono di base siccome il compiler le offre_

- `CFLAGS` : (Flag Compilatore C):
    - '-02' : Il compilatore ottimizza il codice

    - '-g' : Include i simboli di debug nel file .asm, serve per il debug con GDB

    - '-fstack-protector-strong' : Il compilatore aggiunge il controllo tramite canary per stack overflow

    - '-mno-tls-direct-seg-refs' : Disattiva l'uso di segmenti TLS (Thread-Local Storage => memoria locale del singolo thread)
      specifico per arch x86 a 32bit, impedisce al compiler di generare codice che accede alla memoria locale dei thread, dunque di usare %fs, %gs
      Per accedere velocemente alle variabili locali TLS la CPU usa registri specifici %fs, %gs.
      All'avvio del kernel i registri come %fs e %gs non sono configurati e puntano a valori random, usarli sarebbe pericoloso

    - '-fno-pic' : Disabilita la generazione di codice indipendente dalla posizione, il compiler si basa solo su memoria ad indirizzo assoluto e fisso (scelto nella fase di linking)
      serve siccome MULTIBOOT si aspetta un header nel primo MB di memoria e con fPIC attivo il codice va in posizioni randomiche

## compilazione

- `i686-elf`:
    - 'i686' : Rappresenta l'architettura della CPU target (32-bit)
    - '-elf' : Il formato dei file eseguibili e oggetti. Significa bare-metal, senza un OS di destinazione, nessua libreria standard
