def_EHelper(jal) {
  rtl_li(s, ddest, s->snpc);
  rtl_addi(s, s0, id_pc, id_src1->simm);
  rtl_jr(s, s0);
}

def_EHelper(jalr) {
  rtl_li(s, ddest, s->snpc);
  rtl_addi(s, s0, dsrc1, id_src2->simm);

  // setting the LBS of the result to zero
  // rtl_andi(s, s0, s0, 0xfffffffe);

  rtl_jr(s, s0);
}

def_EHelper(beq) {
  rtl_addi(s, s0, id_pc, id_dest->simm);
  rtl_jrelop(s, RELOP_EQ, dsrc1, dsrc2, *s0);
}

def_EHelper(bne) {
  rtl_addi(s, s0, id_pc, id_dest->simm);
  rtl_jrelop(s, RELOP_NE, dsrc1, dsrc2, *s0);
}

def_EHelper(blt) {
  rtl_addi(s, s0, id_pc, id_dest->simm);
  rtl_jrelop(s, RELOP_LT, dsrc1, dsrc2, *s0);
}

def_EHelper(bge) {
  rtl_addi(s, s0, id_pc, id_dest->simm);
  rtl_jrelop(s, RELOP_GE, dsrc1, dsrc2, *s0);
}

def_EHelper(bltu) {
  rtl_addi(s, s0, id_pc, id_dest->simm);
  rtl_jrelop(s, RELOP_LTU, dsrc1, dsrc2, *s0);
}

def_EHelper(bgeu) {
  rtl_addi(s, s0, id_pc, id_dest->simm);
  rtl_jrelop(s, RELOP_GEU, dsrc1, dsrc2, *s0);
}
