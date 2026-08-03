
#ifndef _KERNEL_GDT_H
#define _KERNEL_GDT_H 1

// ACCESS_BYTE
//
// little-endian => il primo bit è a destra
// 7 bit => il segmento è in RAM?
// 6-5 bit / DPL => privilegio (00 KERNEL, 11 USER)
// 4 bit => è un segmento di sistema (0) oppure codice/data (1)?
// 3 bit => è eseguibile? (codice (1) o dati(0))
// 2 bit =>
//          se sono dati (3 bit posto a 1) => direzione crescita segmento: verso
//          l'alto (0 bit) o basso (1 bit)? (stack-like o heap-like) si usa
//          sempre 0, 1 era in passato
//
//          se è codice (3 bit posto a 0) => Eseguibile solo dal proprio livello
//          DPL (0 bit) oppure eseguibile anche da livelli minori/uguali (1
//          bit). Usato in passatto ma oggi sempre 0, si usano syscall
// 1 bit =>
//          se sono dati (3 bit posto a 1) => permette scrittura (1 bit) o no
//
//          se è codice (3 bit posto a 0) => permette alla cpu la lettura di
//          valori/constanti presenti nel codice(1 bit) o no
//
// 0 bit => la cpu lo imposta a 1 quando sta usando l'entry, la si imposta a 0
// sempre

//
// FLAGS
//
// 7 bit => se 1 il limite (limit di 20 bit) dell'ACCESS_BYTE moltiplicato per
//          4KB => 0xFFFFF x 4KB(4096)
//
// 6 bit => se 1 dice modalità a 32-bit protected mode altrimenti se 0 usa real
//          mode a 16bit
//
//          REAL MODE: 1 MB Ram, No ring no GDT, puoi andare dovunque nella
//          memoria senza limiti.
//
//          PROTECTED MODE: 4GB Ram, memoria protetta da
//          GDT e Ring, programmi isolabili dal kernel.
//
// 5 bit => 0 se arch 32bit, 1 se arch 64bit
//
// 4 bit => sempre 0
//

#define GDT_ENTRY_NULL 0x0

// Kernel Code: Ring 0, Executable/Readable, Base=0, Limit=4GB
#define GDT_ENTRY_KERNEL_CODE 0x00CF9A000000FFFFULL

// Kernel Data: Ring 0, Read/Write, Base=0, Limit=4GB
#define GDT_ENTRY_KERNEL_DATA 0x00CF92000000FFFFULL

#define KERNEL_CODE_SELECTOR 0x08 // Index 1 nella GDT (1 * 8 byte)
#define KERNEL_DATA_SELECTOR 0x10 // Index 2 nella GDT (2 * 8 byte)

#endif
