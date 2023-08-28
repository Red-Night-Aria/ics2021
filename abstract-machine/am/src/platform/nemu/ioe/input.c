#include <am.h>
#include <nemu.h>
#include "klib.h"

#define KEYDOWN_MASK 0x8000

void __am_input_keybrd(AM_INPUT_KEYBRD_T *kbd) {
  //printf("enter\n");
  uint32_t scancode = inl(KBD_ADDR);
  //printf("scancode: %d\n", scancode);
  kbd->keydown = scancode & KEYDOWN_MASK;
  kbd->keycode = scancode & (~KEYDOWN_MASK);
}
