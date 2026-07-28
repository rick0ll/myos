
#include <kernel/panic.h>

// Evita falsi allarmi (Warning del compilatore):
// Ottimizzazione
__attribute__((__noreturn__)) void panic(const char *msg) {

  log_fatal(msg);
  /*
   * Blocco di sicurezza estremo per congelare la CPU:
   * - cli: Disabilita le interruzioni hardware (ignora timer, tastiera, ecc.)
   *        per evitare che l'hardware svegli la CPU o esegua driver corrotti.
   * - hlt: Mette la CPU in stato di sonno profondo (sospende il clock interno),
   *        riducendo il consumo di energia allo 0% (evita di far girare le
   * ventole).
   * - while(1): Ciclo di sicurezza. Se un'interruzione critica non mascherabile
   *             dovesse svegliare la CPU, questa viene immediatamente
   * riaddormentata.
   */
  while (1) {
    asm("cli; hlt");
  }

  __builtin_unreachable();
}
