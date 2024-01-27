#include <isa.h>

void isa_reg_display();

word_t isa_raise_intr(word_t NO, vaddr_t epc) {
  // Log("NO: %d", NO);
  // isa_reg_display();
  cpu.mcause = NO;
  cpu.mepc = epc;
  return cpu.mtvec;
}

word_t isa_query_intr() {
  return INTR_EMPTY;
}
