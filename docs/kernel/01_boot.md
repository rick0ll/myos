# SPIEGAZIONE

**TLDR;**
Il file serve a preparare il sistema operativo al linguaggio C.
boot.S serve a permettere il funzionamento del bootloader, preparare la stack per C, creare le GDT,portare il kernel a higher half e inizializzare il paging

## INCLUDE

- 'multiboot2.h' : Contiene costanti utili per l'header multiboot2 oltre che struct utili per C in seguito
- 'kernel/gdt.h' : Contiene costanti utili per la creazione del GDT
- 'kernel/vmm.h' : Contiene costanti utili per la gestione del higher half kernel
- 'kernel/memory.h' : Contiene costanti utili per l'inizializzazione del paging

## COMANDI COMUNI

**Per T&TD**

- `.section {nome}` : Definisce una sezione. Il linker utilizza il nome delle sezioni per organizzarle.

- `.align {size}` : Allinea la memoria a {size}. E' obbligatorio allineare la stack a 16bit, siccome il codice generato dal compilatore si aspetta la stack a 16bit-alligned.
  Serve anche per multiboot2 il quale avanzando a multipli di 8bit alla volta necessita allineamento per leggere correttamente i dati.
  Inoltre per vincolo hw la CPU si aspetta che il registro CR3, i descrittori PDE e PTE siano a indirizzi con 0x000 alla fine (multiplo di 4kb)

- `.quad {var}` : Definisce varibili di 8byte

- `.long {var}` : Definisce variabile di 4byte

- `.short {var}` : Definisce variabile di 2byte

- `{label}:` : Definisce una label a cui si può riferire l'indirizzo, sono come variabili con valore l'indirizzo di quella posizione di codice

- `.global {label}` : Di norma le label sono private al file ma con .global una label può essere usata in altri file linkati (file .asm, .c, e il linker stesso)

- `.skip {size}` : Salta size spazi bit di memoria

- `[comando]l` : Comando per operazioni a 32bit

## SEZIONI

### .BOOT

**Sezione che contiene l'header di multiboot2 con i tag di configurazione e le informazioni richieste al bootloader**

- `.section boot, "ax"` :
    - `ax` : Sta per Allocatable (a) e Executable (x). Mi assicuro che la sezione boot venga caricata in memoria
      durante l'esecuzione del programma e che sia considerato una sezione da eseguire

#### `multiboot_header:`

**Parte della sezione .boot che contiene i campi richiesti e i campi aggiutivi dell'header multiboot2**

##### Campi Richiesti

1. **Magic Number (`0xE85250D6`)**

- Valore magico a 32-bit imposto dalla specifica Multiboot2 per identificare univocamente il kernel come compatibile.

2. **Architettura Target (`architecture`)**

- Specifica l'ISA della CPU per cui eseguire la procedura di boot (es. `0` per architettura i386 / 32-bit Protected Mode).

3. **Header Length (`header_length`)**

- Dimensione complessiva in byte dell'intero header Multiboot2 (compresi i campi base, tutti i tag di richiesta e il tag di chiusura finale `type 0`).

4. **Checksum (`checksum`)**

- Campo a 32-bit calcolato secondo la formula di verifica:

$$\text{checksum} = -(\text{magic} + \text{architecture} + \text{header\_length})$$

- La somma aritmetica a 32-bit di questi 4 campi deve risultare esattamente `0` (`0x00000000`), permettendo al bootloader di convalidare l'integrità dei dati prima del caricamento.

##### Campi Aggiutivi

**Sono i campi dell'header nei quali viene richiesto tramite l'uso di tag delle configurazioni specifiche per il sistema operativo**

Ogni tag presenta la seguente struttura di base assieme ad altri elementi specifici per ogni tipo di tag.

(base_tag_structure)

```asm
.short NUMERO_IDENTIFICATIVO_DEL_TIPO_TAG_RICHIESTA
.short FLAG_MANDATORY_OR_OPTIONAL
.long  LUNGHEZZA_TAG
```

1. **Entry point tag**: Tag che definisce il punto in cui la cpu deve iniziare a eseguire il codice a seguito del bootloading

```asm
(base_tag_structure)
.long ENTRY_POINT_ADDR
```

2. **Module alignment tag** : Dichiara che i moduli (File caricati prima del kernel) caricati in memoria siano allineati a 4KB

```asm
(base_tag_structure)
```

3. **Information request tag** : Richiede informazioni al bootloader

```asm
(base_tag_structure)
.long INFO_01
.long INFO_02
.long INFO_N
```

###### Information tags

- **Command line info** : Richiede di passare tramite registro la stringa passata da riga di comando (tipo argv)
- **RAM Modules address info** : Richiede l'indirizzo di ogni modulo in RAM
- **Memory Map Info** : Richiede una mappa della memoria RAM
- **ELF Symbols** : Passa l'indirizzo della tabella degli headers ELF
  _La tabella dei simboli ELF è una struttura dati che associa ad ogni identificatore del codice (funzioni, variabili globali, variabili statiche)_
  _il rispettivo indirizzo di memoria._
  _Molto utile per stack-trace, necessario al linker per calcolare dove si trova ogni funzione e collegare le chiamate tra i file (\*.o) in un unico file_

- **Framebuffer Info** : Richiede informazioni utili per la gestione del Framebuffer
- **SMBIOS Table** : Richiede la tabella SMBIOS
  _Insieme di tabelle dati create dal firmware (BIOS/UEFI) e presenti in RAM all'avvio dell'OS_
  _Fornisce al kernel una descrizione dettagliata dei componenti fisici della macchina (modello e produttore motherboard, modello CPU, numero di slot RAM presenti, UUID del sistema)_
  _Permette di avere informazioni aggiuntive sul sistema_

- **ACPI Table v1** : Richiede la v1 della tabella ACPI
  _Insieme di tabelle di configurazione e di codice eseguibile.
  Nelle tabelle ci sono informazioni necessarie per il controllo degli interrupt, per l'utilizzo del multicore, controllo energetico/termico, timer hardware etc_
  _Tramite il codice eseguibile vengono gestiti gli interrupt_

- **ACPI Table v2** : Richiede la v2 della tabella ACPI
  _La versione 2 è utilizzata nei sistemi moderni a 64-bit, il riferimento alle tabelle è con puntatori a 64bit.
  Le tabelle possono essere ad indirizzi >4GB (limite di RAM per 32-bit arch)._
  _La versione 2 è quella più usata ma la versione 1 viene comunque gestita in caso di fallback_

- **Networking info** : Richiede informazioni sulla scheda di rete

4. **END REQUEST TAG** : Dichiara la fine dei tag richiesti

```asm
(base_tag_structure)
```

#### \_start

- `global _start`: Rende la label \_start visibile ad altri file come il file del linker.

- `cmpl $MAGIC_MULTIBOOT2_PROOF_NUMBER, %eax`: Verifica se viene usato multiboot2 come bootloader verificando che il registro `ax` contenga uno specifico numero.

- `jne bad_bootloader` : Nel caso non siano uguali i valori precedentemente comparati allora si salta alla label `bad_bootloader`.

##### bad_bootloader

- `movl $0x000B8000, %edi`: Salva l'indirizzo della VGA nel registro `edi` (Extender Destination Index, usato spesso per rifermenti a zone di memoria)
- `movw $0x2F41, (%edi)`: Scrive sull'indirizzo salvato su `edi` una lettera 'B' bianca su sfondo blu. (Per debug)(Non posso fare direttametne valore lettera - indirizzo senz usare edi?)
- `cli`: (Clear Interrupt Flag) Disabilito gli interrupt mettendo a 0 il regitro `IF` (Interrupt Flags). La CPU ignorerà ogni hardware input (pk la cpu, non è l'os a gestirli?)
- `hlt`: (Halt) Mette la CPU in uno stato dormiente (low-power sleep state (C1 state)). Ferma ciclo FDE (Fetch, Decode, Execute) quindi il susseguirsi di istruzioni.
- `jmp bad_bootloader` : Non servirebbe siccome `hlt` nel caso la cpu riprenda l'esecuzione ritorna alla label bad_bootloader.

- `movl $(stack_top - KERNEL_VIRT_BASE), %esp`: Salvo nel registro `esp` (Extended Stack Pointer) l'indirizzo della parte alta della stack `stack_top` - `KERNEL_VIRT_BASE`
  Siccome la definizione della stack si trova in `.BSS` che è posto nella zona higher half (3GB in su) devo sottrarre l'indirizzo virtuale,
  almeno fino a che il paging non è estato attivato.

- `andl $-16, %esp` : Allineo `esp` a 4bit come richiesto dalla CPU.
  16 => 00...10000; Quando viene usato '-' allora si invertono i bit: !16 => 11...01111; e viene sommato 1: -16 => 11...10000;

> Per gestire il segno meno si usa un bit di segno, il bit più significativo (Little Endian), e viene considerato il valore di quel bit (se posto a 1) con segno -
> Poi vengono attivati bit a destra del bit più significativo che vengono considerati positivi e quidi sommatti al primo valore negativo.
> Si ottengono così numeri negativi da -1 a 2^(n-1) (n = numero di bit usati dalla variabile)
> (n-1 siccome il bit più significativo rappresenta il segno, se primo bit a 1 allora la cpu lo pensa come num negativo e torna indietro da 2^n fino a 2^0)

- `add KERNEL_VIRT_BASE, %ebx` : Rendo l'indirizzo contenuto in `ebx` un indirizzo virtuale del kernel.
  `ebx` contiene il riferimento alle informazioni che sono state richieste a multiboot2.

- `pushl ebx` : Salvo `ebx` sulla stack così da poterlo usare in kernel_main.

- `mov $(boot_page_directory - KERNEL_VIRT_BASE), %ebx`: Passo il riferimento all'indirizzo fisico della tabella PDT al registro `ebx`.
  Il linker ha spostato le parti di boot.S nell'indirizzo fisico dopo primo MB ma le label non vengono spostate dal linker, dunque puntano all'indirizzo virtuale.
  Il paging ancora non c'è ma anche se ci fosse `cr3` richiede l'indirizzo fisico, non passa tramite MMU.

- `mov %ebx, %cr3` : Passo il riferimento della PDT al registro di controllo `cr3`, usato dalla CPU per il paging.

- `mov %cr4, %ecx`: Self-explanatory (`cr4` contiene flag associate al paging)

- `or $0x00000010, %ecx` : Attivo il bit indicante pagine da 4MB. Lo faccio solo nella fase di inizializzazione higher half così non devo creare sia Page Directory che PTE

- `mov %ecx, %cr4`: Self-explanatory

- `mov %cr0, %ecx` : Self-explanatory (`cr0` contiene varie flag)

- `or $0x80000000, %ecx` : Attivo il bit che indica l'attivazione del paging.

- `mov %ecx, %cr0`: Self-explanatory

- `jmp higher_half`: Vado sull'indirizzo di memoria che `ecx` contiene

- `.size _start, . - _start` : Indico a multiboot2 la dimensione di \_start, utile per il debug con la tabella dei simboli ELF (so poco niente)

## .BSS

**Sezione che si occupa di definire la dimensione della stack (zona di memoria non inizializzatta) del kernel**

- `.align STACK_ALLIGNMENT` : Allinea la stack a 16byte come richiesto dal compilatore C
- `.skip STACK_SIZE` : Salta STACK_SIZE bit di memoria. E' la zona di memoria dove risiederà la stack
- `stack_bottom: ` : Indica la parte bassa della stack
- `stack_top` : Indica la parte alta della stack

## .DATA

**Sezione con i dati inizializzati necessari per la GDT e la Page Directory iniziale.**

### 1. Definizione della GDT in RAM

- **`.align 8`**: Allinea la sezione in memoria a **8 byte** (non bit), come richiesto per le prestazioni e la specifica architetturale.
- **`.quad GDT_ENTRY_NULL`**: (8 byte) Il Null Descriptor. La prima entry va sempre posta a zero per intercettare segmenti non validi.
- **`.quad GDT_ENTRY_KERNEL_CODE`**: (8 byte) Entry di Ring 0 (Kernel) per l'esecuzione del codice.
- **`.quad GDT_ENTRY_KERNEL_DATA`**: (8 byte) Entry di Ring 0 (Kernel) per lettura/scrittura dati.

### 2. Il GDT Descriptor (GDTR)

- **`.short gdt_end - gdt_start - 1`**: (Limit) Indica la dimensione totale in byte della GDT, meno 1.
- **`.long gdt_start`**: (Base) Indica l'indirizzo di partenza della tabella.

### 3. Page Directory iniziale (Boot Page Directory)

Prepara un array di 1024 PDE (Page Directory Entries) per avviare il kernel.

- **`.long KERNEL_PAGE_BOOT`**: Entry 0. Mappa i primi 4MB di RAM (Identity Mapping: `0x00000000 -> 0x00000000`). Serve perché la CPU sta ancora eseguendo codice in memoria bassa prima del salto (jump).
- **`.fill 767, 4, 0`**: Riempie 767 entry consecutive con il valore 0 (ciascuna di 4 byte). Queste pagine non sono mappate.
- **`.long KERNEL_PAGE_BOOT`**: Entry 768. Mappa gli stessi 4MB fisici all'indirizzo virtuale `0xC0000000` (Higher Half Kernel). 768 \* 4MB = 3GB esatti.
- **`.fill 255, 4, 0`**: Riempie le restanti 255 entry con 0 per completare la tabella di 1024 elementi (4KB totali).

## GDT (Global Descriptor Table)

> **TL;DR**
> La GDT è una tabella di ruoli (segmenti). Tramite questa tabella, la CPU controlla i permessi di esecuzione del codice e di accesso ai dati.
> In passato (nei vecchi OS x86), la GDT veniva usata per isolare fisicamente le zone di memoria di ogni programma. Nei sistemi operativi moderni che utilizzano il **Paging**, la segmentazione viene di fatto "annullata" (_Flat Memory Model_), e la GDT diventa unicamente un gestore di ruoli e livelli di privilegio (Ring 0 per il Kernel, Ring 3 per lo User Space).

### 1. GDT Descriptor (GDTR)

Il registro della CPU `GDTR` ha bisogno di un puntatore di **48 bit (6 byte)** per sapere dove si trova la tabella e quanto è grande in memoria.

```asm
.short limit    // (16 bit) Dimensione totale della tabella in byte, meno 1
.long  base     // (32 bit) Indirizzo virtuale in cui inizia la GDT
```

### 2. GDT Entry (Segment Descriptor)

Ogni entry della GDT è lunga 64 bit (8 byte) e descrive le caratteristiche di un singolo segmento.

**Perché la prima entry (Index 0) è sempre NULL?**

L'architettura x86 impone che il primo descrittore della GDT (offset 0x00) sia completamente azzerato.
Questo Null Descriptor funge da sistema di sicurezza hardware:
se un registro di segmento (CS, DS, SS, ecc.) non viene inizializzato o viene accidentalmente impostato a zero,
qualsiasi tentativo di usarlo per accedere alla memoria genererà immediatamente un'eccezione #GP (General Protection Fault),
prevenendo corruzioni silenziose della memoria.

Per motivi storici legati alla retrocompatibilità con il processore 80286, i bit della struttura sono sparsi e frammentati:

```asm
.byte base_high
.byte flags_and_limit_high
.byte access_byte
.byte base_middle
.short base_low
.short limit_low
```

    Base (32 bit): Si ottiene unendo Base High, Base Mid e Base Low.
                    Indica l'indirizzo virtuale di partenza del segmento.
                    Nel Flat Memory Model di un kernel moderno, questo valore è sempre 0x00000000.

    Limit (20 bit): Si ottiene unendo Limit High e Limit Low. Definisce l'indirizzo massimo raggiungibile da questo segmento.
                    Nel Flat Memory Model, combinato con la flag Granularity, viene impostato al valore massimo 0xFFFFF.

#### Access Byte (8 bit)

Spiega alla CPU chi può accedere al segmento e come.

    Bit 0 (Accessed): La CPU lo pone automaticamente a 1 quando il segmento viene usato. Va inizializzato a 0.

    Bit 1 (Read/Write):

        Se Code Segment: 1 = Permette la lettura (il codice è sempre eseguibile, ma così puoi anche leggerne le costanti).

        Se Data Segment: 1 = Permette la scrittura (i dati sono sempre leggibili).

    Bit 2 (Direction/Conforming):

        Se Code Segment: 0 = Il codice può essere eseguito solo dal livello di privilegio esatto (DPL). 1 = Può essere eseguito anche da livelli inferiori.

        Se Data Segment: 0 = Il segmento cresce verso l'alto (normale). 1 = Cresce verso il basso (storicamente usato per gli stack).

    Bit 3 (Executable): 1 = Segmento di Codice. 0 = Segmento di Dati.

    Bit 4 (Descriptor Type): 1 = Segmento standard (Codice/Dati). 0 = Segmento di Sistema (usato per il TSS - Task State Segment, una struttura che servirà in futuro per gestire gli interrupt dallo User Space).

    Bit 5-6 (DPL - Descriptor Privilege Level): Livello di privilegio del segmento. 0 = Kernel (Massimo privilegio), 3 = User Applications (Minimo privilegio).

    Bit 7 (Present): 1 = Il segmento è valido e presente in memoria.

#### Flags (4 bit)

Questi 4 bit si uniscono al "nibble" (4 bit) superiore del Limit.

    Bit 0 (AVL - Available): A disposizione per il software di sistema. Normalmente si lascia a 0.

    Bit 1 (L - Long / 64-bit flag): 1 se il segmento contiene codice nativo a 64 bit. Per un OS x86 a 32 bit, deve essere 0.

    Bit 2 (D/B - Size Flag): Indica la dimensione operativa di default.

        0 = 16-bit (Modalità reale/MS-DOS).

        1 = 32-bit (Protected Mode). Dice alla CPU che i registri e gli indirizzi standard sono a 32 bit (es. usa EAX invece di AX).
        Per il tuo kernel va sempre a 1.

    Bit 3 (G - Granularity): Moltiplicatore del Limit.

        0 = Il limite è calcolato in singoli Byte (Dimensione max: 1 MB).

        1 = Il limite è calcolato in Pagine da 4KB. (In questo modo, il limite massimo di 0xFFFFF moltiplicato per 4096 dà esattamente 4 GB,
        coprendo l'intero spazio di indirizzamento a 32 bit).

## PAGING

> **TL;DR**
> È un sistema di gestione della memoria che permette ad ogni processo di avere uno spazio di **memoria virtuale continuo**, senza che la memoria fisica sottostante debba esserlo.
> La paginazione garantisce anche l'isolamento della memoria (un processo non può leggere le pagine di un altro) e il controllo dei permessi a livello di singola pagina (Read-Only, User/Kernel).

---

### 1. MMU (Memory Management Unit)

Si tratta di un componente hardware della CPU che si occupa di tradurre al volo gli indirizzi di memoria virtuale in indirizzi fisici e di imporre i controlli sui permessi.

Utilizza il **TLB (Translation Lookaside Buffer)**, una velocissima cache hardware interna alla CPU che memorizza le ultime traduzioni effettuate, evitando di dover rileggere le tabelle in RAM ad ogni istruzione.

> **Importante:** La CPU non aggiorna il TLB automaticamente se una Page Table viene modificata! L'accuratezza del TLB dipende dal Kernel, che deve invalidarlo quando altera le mappature. In x86 si fa in due modi:
>
> - **Invalidare una singola pagina (PD Entry : 4MB/ PT Entry 4KB):** Tramite l'istruzione assembly `invlpg (PDE index)` oppure `invlpg (indirizzo_virtuale)`.
> - **Flush totale del TLB:** Ricaricando l'intero registro `CR3` (`mov %cr3, %eax; mov %eax, %cr3`).

#### Architettura MMU (x86 usa Hardware-managed TLB)

- **Hardware-managed TLB:** In caso di _cache-miss_ (indirizzo non presente nel TLB), la CPU (non l'OS) "cammina" automaticamente lungo le Page Tables in RAM (Page Walk). Se trova la pagina, aggiorna il TLB. Se la pagina non è valida (bit Present = 0), la CPU genera un'eccezione di **Page Fault**.
- _(Esistono architetture Software-managed, come MIPS o RISC-V base, dove è il Kernel a dover gestire a mano i cache-miss del TLB)._

#### Come viene diviso un indirizzo virtuale a 32-bit

In modalità x86 standard (pagine da 4KB), un indirizzo di memoria (es. `0xC010204A`) viene smontato dalla MMU in tre parti:

| Bit                  | Scopo                                                 |
| :------------------- | :---------------------------------------------------- |
| **22 - 31** (10 bit) | Indice nella **Page Directory** (da 0 a 1023).        |
| **12 - 21** (10 bit) | Indice nella **Page Table** (da 0 a 1023).            |
| **0 - 11** (12 bit)  | Offset finale all'interno della pagina fisica di 4KB. |

---

### 2. Page Directory

È un array allineato in memoria a 4KB contenente 1024 **Page Directory Entries (PDE)**.
Puntando il registro `CR3` all'indirizzo fisico di questa directory, si attiva la paginazione.
Ogni PDE può puntare a un'altra tabella (Page Table) oppure mappare direttamente un enorme blocco di memoria da **4MB**.

#### Page Directory Entry (PDE)

Ogni entry è lunga 32 bit. La sua struttura cambia leggermente a seconda del flag _Page Size (PS)_ al bit 7.

##### Caso A: Puntatore a una Page Table (Page Size = 0)

Mappa indirettamente 4MB di memoria divisi in 1024 pagine da 4KB.

| Bit       | Nome                  | Valore e Significato                                                                                                                                                                                                                               |
| :-------- | :-------------------- | :------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| **0**     | **Present** (P)       | `1` = La Page Table esiste. `0` = Genera Page Fault se vi si accede.                                                                                                                                                                               |
| **1**     | **Read/Write** (R/W)  | `1` = Lettura/Scrittura. `0` = Sola lettura (Write Protection in CR0).                                                                                                                                                                             |
| **2**     | **User/Super.** (U/S) | `1` = Accessibile allo User Space (Ring 3). `0` = Solo Kernel (Ring 0-2).                                                                                                                                                                          |
| **3**     | **Write-Through**     | Gestione Cache. `1` = Write-Through, `0` = Write-Back.                                                                                                                                                                                             |
| **4**     | **Cache Disable**     | Gestione Cache. `1` = Cache disabilitata per questa pagina.                                                                                                                                                                                        |
| **5**     | **Accessed** (A)      | **Spiegazione:** La CPU lo mette a `1` in automatico quando un programma legge o scrive questo indirizzo. Serve all'OS per capire quali pagine sono inutilizzate da tempo ed espellerle su disco in caso di RAM esaurita (Swapping/Algoritmo LRU). |
| **6**     | **Ignorato**          | Riservato.                                                                                                                                                                                                                                         |
| **7**     | **Page Size** (PS)    | `0` = Indica che questa entry punta a una Page Table (pagine da 4KB).                                                                                                                                                                              |
| **8-11**  | **Available**         | A disposizione del Kernel per metadati custom.                                                                                                                                                                                                     |
| **12-31** | **Indirizzo Fisico**  | I 20 bit superiori dell'indirizzo fisico della **Page Table** associata.                                                                                                                                                                           |

##### Caso B: Pagina singola da 4MB (Page Size = 1)

Richiede che il bit PSE (Page Size Extension) nel registro `CR4` sia a `1`. Molto usata per mappare il Kernel all'avvio.

| Bit       | Nome                 | Valore e Significato                                                                                                                                                                                  |
| :-------- | :------------------- | :---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| **0-5**   | Uguali al Caso A.    | Accesso, Permessi, Cache, Accessed.                                                                                                                                                                   |
| **6**     | **Dirty** (D)        | **Spiegazione:** La CPU lo pone a `1` quando qualcuno **scrive** su questa pagina. Serve all'OS per sapere se i dati sono stati modificati e devono essere salvati su disco prima di liberare la RAM. |
| **7**     | **Page Size** (PS)   | `1` = Indica che la entry mappa direttamente 4MB fisici.                                                                                                                                              |
| **8**     | **Global** (G)       | `1` = Non invalidare la traduzione nel TLB quando si fa il flush di `CR3` (usato per le pagine fisse del Kernel).                                                                                     |
| **12**    | **PAT**              | _Page Attribute Table_: Feature avanzata per specificare il tipo di caching (es. Write-Combining per la memoria video). Normalmente a `0`.                                                            |
| **22-31** | **Indirizzo Fisico** | I 10 bit superiori dell'indirizzo fisico di base del blocco da 4MB. (I bit 13-21 sono riservati a 0).                                                                                                 |

---

### 3. Page Table

È un array allineato a 4KB contenente 1024 **Page Table Entries (PTE)**. Definisce la traduzione finale per le singole pagine da 4KB.

#### Page Table Entry (PTE)

Ogni entry è di 32 bit e punta all'indirizzo fisico reale in RAM.

| Bit       | Nome                  | Valore e Significato                                                        |
| :-------- | :-------------------- | :-------------------------------------------------------------------------- |
| **0**     | **Present** (P)       | `1` = La pagina è caricata in RAM fisica.                                   |
| **1**     | **Read/Write** (R/W)  | `1` = R/W. `0` = Read-Only.                                                 |
| **2**     | **User/Super.** (U/S) | `1` = User Space. `0` = Kernel Space.                                       |
| **3**     | **Write-Through**     | `1` = Write-Through, `0` = Write-Back.                                      |
| **4**     | **Cache Disable**     | `1` = Disabilita cache per la pagina.                                       |
| **5**     | **Accessed** (A)      | Posto a `1` dalla CPU in caso di lettura/scrittura (utile per lo Swapping). |
| **6**     | **Dirty** (D)         | Posto a `1` dalla CPU in caso di **scrittura** (utile per lo Swapping).     |
| **7**     | **PAT**               | Page Attribute Table per caching avanzato.                                  |
| **8**     | **Global** (G)        | `1` = Mantiene la pagina nel TLB anche se si ricarica `CR3`.                |
| **9-11**  | **Available**         | Uso libero per il sistema operativo.                                        |
| **12-31** | **Indirizzo Fisico**  | I 20 bit superiori dell'indirizzo fisico effettivo della                    |

**TLDR;**
E' un sistema di gestione della memoria che permette ad ogni processo di avere una zona contigua di memoria virtuale senza che fisicamente sia contigua.
La paginazione permette anche a una gestione degli accessi a livello di pagine e isolamento da una pagina a un'altra.

### .TEXT

**TLDR;**

Zona di memoria dove risiede il codice da eseguire. Preparo la stack per higher half kernel e chiamo kernel main

- `higher_half:` : Label
- `add $KERNEL_VIRT_BASE, %esp`: Sistemo lo stack pointer per funzionare con indirizzamento virtuale aggiungendo l'indirizzo virtuale del kernel.
- `movl $0, boot_page_directory` : Metto a '0', cancello, la prima entry del paging che ho usato per prima di spostare il kernel a un indirizzo virtuale.
  Lo metto a '0' così non c'è rischio che un programma che riceve indirizzo virtuale al PDE[0] scriva nel codice del kernel.

- `invlpg (0)`: Invalida solo prima entry di PDE, siccome era attivo PDE con 4MB.

- `lgdt (gdt_descriptor)` : Comando specifico per caricare sul registro `GDTR` (Global Descriptor Table Register) l'indirizzo del descrittore di tabella GDT.

- `mov $KERNEL_DATA_SELECTOR, %ax` : Metto in `ax` l'offset per arrivare alla sezione di `Data Selector` partendo dal GDT start

```asm
mov %ax, %ds
mov %ax, %es
mov %ax, %fs
mov %ax, %gs
mov %ax, %ss
```

Aggiorno i registri di segmento (finiscono con 's' segment).
La cpu allora per ogni registro va all'indice GDT che gli ho passato tramite offset e salva i permessi letti in una parte di ogni registro (Descriptor Cache/Shadow Register)
Così sa che tipo di permesso ogni registro ha.

- `ljmp $KERNEL_CODE_SELECTOR, $ip_set` : Esegue un long jump verso la label `ip_set` e
  aggiorna anche il registro di segmento del codice `CS` con il valore del offset per il descriptor del codice kernel.
  Non si può fare direttamente `mov $KERNEL_CODE_SELECTOR, cs`

#### ip_set

- `call _init` : Chiama i costruttori globali. Vedi crti.S, crtn.S.
- `call kernel_main` :Chiama il kernel main, il quale ricevo come parametri i valori nella stack (addr di multiboot2 info).
