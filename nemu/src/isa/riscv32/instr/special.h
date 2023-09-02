def_EHelper(inv) {
  rtl_hostcall(s, HOSTCALL_INV, NULL, NULL, NULL, 0);
}

def_EHelper(nemu_trap) {
  rtl_hostcall(s, HOSTCALL_EXIT, NULL, &gpr(10), NULL, 0); // gpr(10) is $a0
}

def_EHelper(csrrw) {
  // rtl_addi(s, ddest, dsrc1, 0);
  rtl_mv(s, dsrc1, dsrc2);
}

def_EHelper(csrrs) {
  rtl_mv(s, ddest, dsrc1);
  // rtl_addi(s, dsrc1, dsrc2, 0);
}

def_EHelper(ecall) {
  rtl_j(s, isa_raise_intr(gpr(17), s->pc));
}

def_EHelper(mret) {
  rtl_addi(s, s0, &(cpu.mepc), 4);
  rtl_jr(s, s0);
}