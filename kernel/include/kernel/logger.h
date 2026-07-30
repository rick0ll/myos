#ifndef KERNEL_LOG_PRINT
#define KERNEL_LOG_PRINT 1

#include <stdint.h>
#include <sys/cdefs.h>

#ifndef GLOBAL_LOG_LEVEL
#define GLOBAL_LOG_LEVEL LOG_LEVEL_TRACE
#endif

#define LOG_LEVEL_TRACE 0
#define LOG_LEVEL_DEBUG 1
#define LOG_LEVEL_INFO 2
#define LOG_LEVEL_WARN 3
#define LOG_LEVEL_ERROR 4
#define LOG_LEVEL_FATAL 5

// ##__VA_ARGS__ è la parola chiave speciale che dice al preprocessore: "Prendi
// tutti gli argomenti extra che hai ricevuto al posto dei tre punti e incollali
// esattamente qui".

int kernel_log_internal(uint8_t color, const char *__restrict__ level,
                        const char *__restrict__ str, ...);

#define log_trace(fmt, ...)                                                    \
  do {                                                                         \
    if (LOG_LEVEL_TRACE >= GLOBAL_LOG_LEVEL)                                   \
      kernel_log_internal(0x07, "[TRACE] ", fmt, ##__VA_ARGS__);               \
  } while (0)

#define log_debug(fmt, ...)                                                    \
  do {                                                                         \
    if (LOG_LEVEL_DEBUG >= GLOBAL_LOG_LEVEL)                                   \
      kernel_log_internal(0x07, "[DEBUG] ", fmt, ##__VA_ARGS__);               \
  } while (0)

#define log_info(fmt, ...)                                                     \
  do {                                                                         \
    if (LOG_LEVEL_INFO >= GLOBAL_LOG_LEVEL)                                    \
      kernel_log_internal(0x09, "[INFO] ", fmt, ##__VA_ARGS__);                \
  } while (0)

#define log_warn(fmt, ...)                                                     \
  do {                                                                         \
    if (LOG_LEVEL_WARN >= GLOBAL_LOG_LEVEL)                                    \
      kernel_log_internal(0x0E, "[WARN] ", fmt, ##__VA_ARGS__);                \
  } while (0)

#define log_error(fmt, ...)                                                    \
  do {                                                                         \
    if (LOG_LEVEL_ERROR >= GLOBAL_LOG_LEVEL)                                   \
      kernel_log_internal(0x0C, "[ERROR] ", fmt, ##__VA_ARGS__);               \
  } while (0)

#define log_fatal(fmt, ...)                                                    \
  do {                                                                         \
    if (LOG_LEVEL_FATAL >= GLOBAL_LOG_LEVEL)                                   \
      kernel_log_internal(0x4F, "[FATAL] ", fmt, ##__VA_ARGS__);               \
  } while (0)

#endif
