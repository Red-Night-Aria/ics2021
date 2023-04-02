def_EHelper(jal) {
  rtl_li(s, ddest, s->snpc);
  rtl_addi(s, s0, id_pc, id_src1->simm);
  rtl_jr(s, s0);
}

def_EHelper(jalr) {
  rtl_li(s, ddest, s->snpc);
  rtl_addi(s, s0, dsrc1, id_src2->simm);

  // setting the LBS of the result to zero
  rtl_andi(s, s1, s0, 0xfffffffe);

  rtl_jr(s, s1);
}