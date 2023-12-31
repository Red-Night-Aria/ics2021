#include <isa.h>
#include <cpu/difftest.h>
#include "../local-include/reg.h"

bool isa_difftest_checkregs(CPU_state *ref_r, vaddr_t pc) {
  for (int i = 0; i < ARRLEN(ref_r->gpr); ++i) {
    if (ref_r->gpr[i]._32 != cpu.gpr[i]._32) {
      Log("Difftest fail: register %d value is diffrent. DUT: %x, REF: %x", i, ref_r->gpr[i]._32, cpu.gpr[i]._32);
      return false;
    }
  }
  return true;
}

void isa_difftest_attach() {
}
