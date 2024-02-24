#include <common.h>
#include "fs.h"
#include "syscall.h"
#include "device.h"

void do_syscall(Context *c) {
  uintptr_t a[4];
  a[0] = c->GPR1;
  a[1] = c->GPR2;
  a[2] = c->GPR3;
  a[3] = c->GPR4;
  switch (a[0]) {
    case SYS_exit:
      halt(c->GPR2);
      break;
    case SYS_yield:
      c->GPRx = 0;
      yield();
      break;
    case SYS_write:
      c->GPRx = fs_write(a[1], (void*)a[2], a[3]);
      break;
    case SYS_read:
      c->GPRx = fs_read(a[1], (void*)a[2], a[3]);
      break;
    case SYS_lseek:
      c->GPRx = fs_lseek(a[1], a[2], a[3]);
      break;
    case SYS_close:
      c->GPRx = fs_close(a[1]);
      break;
    case SYS_open:
      c->GPRx = fs_open((char*)a[1], a[2], a[3]);
      break;
    case SYS_brk:
      c->GPRx = 0;
      break;
    case SYS_gettimeofday:
      c->GPRx = read_uptime((struct timeval*)a[1]);
      break;
    default: panic("Unhandled syscall ID = %d", a[0]);
  }
  // Log("Syscall %d: Parameters[%p, %p, %p], RetVal %d", a[0], a[1], a[2], a[3], c->GPRx);
}
