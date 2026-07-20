
#ifndef _KERNEL_STACK_PROTECTOR_H
#define _KERNEL_STACK_PROTECTOR_H

#include <stdint.h>

// extern serve per dire che il canary è presente in un file e che sicuramente
// l'ho definito
extern uint32_t __stack_chk_guard;

// in sistemi multithread stack_chk_guard_local è il canary per ogni thread
// finché non adatto il kernel per gestire più thread lo stack_chk_guard è
// globale
#define __stack_chk_guard_local __stack_chk_guard

__attribute__((no_stack_protector, noreturn)) void __stack_chk_fail(void);

#endif
