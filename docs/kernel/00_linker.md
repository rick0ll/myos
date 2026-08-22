# SPIEGAZIONE

## A COSA SERVE IL FILE

**TLDR;**
Il linker serve per unire i vari file (\*.o) in un unico eseguibile.

### COSA FA

1. Unisce le sezioni (.text, .bss, .data) di ogni file compilato (\*.o) in un unico file con le proprie sezioni
2. Compie la risoluzione dei simboli (Symbol Resolution), ossia collega ogni chiamata a funzione alla funzione
3. Definisce il range il cui il codice compilato risiede

Di norma il compilator linka i file basandosi sul sistema operativo sottostante che fornisci il range di indirizzi.

### CODICE

- 'ENTRY(\_start)' : Punto di ingresso del codice. Indica da dove la CPU deve iniziare a eseguire il codice.
  In questo caso il punto di ingresso è la sezione \_start presente in boot.S

- 'SECTIONS' {...} : Definisce le varie sezione del file eseguibile

- '.=1MB' : (Location Counter) Indica che l'indirizzo fisico di caricamento delle sezioni inizia a 1MB. Il primo MB infatti è riservato per gestione hw.

- 'var=.' : Crea una variabile non valorizzata in posizione attuale.

- '.boot ALIGN(4K)' : Posiziona la sezione .boot (presente in boot.S) che deve essere allineata a 4KB (deve essergli multiplo) come richiesto dalla CPU.

- 'KEEP(\*(.boot))' : Il linker elimina automaticamente zone di memoria definite in esso che non vengono referenziate dal codice C (tipo GC). La sezione.boot
  serve per GRUB, di cui il linker non è a conoscenza, dunque serve obbligarlo a non eliminare la sezione.

- '\*(.{sezione})' : Il linker unisc tutte le .{sezione} che trova in ogni file nella stessa posizione.
  Facendo nome_file.estensione(.sezione) hai più granularità.
  Puoi anche fare \*(sezione\*) => qualsiasi sezione che inizia per 'sezione'

- 'AT(...)' : Indica la posizione fisica della sezione, viene usata per distinguere l'indirizzo fisico del kernel (1MB in poi) da quello virtuale (0xC0000000 in poi).
  Il compilatore considererà l'indirizzo virtuale (0xC0000000) ignorando quello che dice AT.

### SEZIONI

- 'boot' : Sezione contenente l'header di Multiboot2, l'inizializzazione della stack, inizializzazione paging, inizializzazione GDT

- 'text' : Sezione con il codice C/asm

- 'rodata' : Sezione con i dati di sola lettura come stringhe (non in stack), variabili const

- 'data' : Sezione con i dati inizializzati (var) di ogni file

- 'bss' : Sezione con i dati globali o statici non inizializzati

- 'COMMON' : Non è una sezione ma un'area di transizione, contiene variabili non inizializzate, nel programma finale COMMON non c'è è fuso con .bss
