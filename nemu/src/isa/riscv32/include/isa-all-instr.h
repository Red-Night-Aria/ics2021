#include <cpu/decode.h>
#include "../local-include/rtl.h"

#define INSTR_LIST(f) f(lui) f(lw) f(sw) f(inv) f(nemu_trap) f(addi) f(auipc) f(jal) f(jalr) \
f(andi) f(ori) f(xori) \
f(add) f(sub) f(slt) f(slti) f(sltu) f(sltui) \
f(beq) f(bne) f(blt) f(bge) f(bltu) f(bgeu) \
f(xor) f(or) f(and) \
f(lh) f(lb) f(sh) f(sb) f(lbu) f(lhu) \
f(slli) f(srli) f(srai) f(sll) f(srl) f(sra) \
f(mul) f(mulh) f(mulhu) f(div) f(divu) f(rem) f(remu) \
f(csrrw) f(csrrs) f(ecall) f(mret)

def_all_EXEC_ID();
