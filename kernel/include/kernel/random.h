

#ifndef _KERNEL_RANDOM_H
#define _KERNEL_RANDOM_H 1

#include <stdint.h>
#include <sys/cdefs.h>

// Funzione che restituisce un numero casuale (random) in base a entropia hw, o
// se fallisce con cicli di clock Questo numero sarà usato per creare il canary
// dello stack.
uint32_t get_random(void);

#endif
