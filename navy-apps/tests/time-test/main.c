#include <stdio.h>
#include <assert.h>
#include "NDL.h"

int main() {
  uint32_t ticker = NDL_GetTicks();
  printf("Current time since boot: %u s %06u us\n", ticker / 1000000, ticker % 1000000);
  uint32_t cur_ticker; 
  int idx = 10;
  while (idx > 0) {
    cur_ticker = NDL_GetTicks();
    if (cur_ticker > ticker + 500000) {
      ticker = cur_ticker;
      printf("Current time since boot: %u s %06u us\n", ticker / 1000000, ticker % 1000000);
      --idx;
    }
  }
  printf("PASS!!!\n");
  return 0;
}
