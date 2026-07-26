

#include <kernel/logger.h>
#include <kernel/parse_info_requests.h>
#include <kernel/stack_protector.h>
#include <kernel/tty.h>

#include <stdint.h>

__attribute__((no_stack_protector)) uint32_t initCanary(void);
