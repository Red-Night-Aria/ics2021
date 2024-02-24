#include "../../libs/libfixedptc/include/fixedptc.h"
#include <stdio.h>

int main() {
  fixedpt a = fixedpt_rconst(1.3);
  fixedpt b = fixedpt_rconst(2.6);
  // const char* const_str[2] = {"FAIL", "SUCC"};
  printf("test muli %s\n", fixedpt_cstr(fixedpt_muli(a, 2), -1));
  printf("test divi %s\n", fixedpt_cstr(fixedpt_divi(b, 2), -1));
  printf("test mul %s\n", fixedpt_cstr(fixedpt_mul(a, b), -1));
  printf("test div %s\n", fixedpt_cstr(fixedpt_div(b, a), -1));
  printf("test abs %s\n", fixedpt_cstr(fixedpt_abs(fixedpt_rconst(-1.3)), -1));
  printf("test floor_1 %s\n", fixedpt_cstr(fixedpt_floor(a), -1));
  printf("test floor_2 %s\n", fixedpt_cstr(fixedpt_floor(fixedpt_rconst(-1.3)), -1));
  printf("test cell_1 %s\n", fixedpt_cstr(fixedpt_ceil(a), -1));
  printf("test cell_2 %s\n", fixedpt_cstr(fixedpt_ceil(fixedpt_rconst(-1.3)), -1));
  printf("test cell_3 %s\n", fixedpt_cstr(fixedpt_ceil(fixedpt_fromint(1)), -1));
  return a;
}