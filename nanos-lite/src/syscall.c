#include <common.h>
#include "syscall.h"

enum {
  SYS_exit,
  SYS_yield,
  SYS_open,
  SYS_read,
  SYS_write,
  SYS_kill,
  SYS_getpid,
  SYS_close,
  SYS_lseek,
  SYS_brk,
  SYS_fstat,
  SYS_time,
  SYS_signal,
  SYS_execve,
  SYS_fork,
  SYS_link,
  SYS_unlink,
  SYS_wait,
  SYS_times,
  SYS_gettimeofday
};

void do_write(Context *c) {
  int fd = c->GPR2;
  char* buf_ptr = (char *)c->GPR3;
  size_t count = c->GPR4;
  // Log("fd: %d, count: %d", fd, count);
  if (fd == 1 || fd == 2) {
    for (size_t i = 0; i < count; ++i) {
      putch(*buf_ptr);
      ++buf_ptr;
    }
    c->GPRx = count;
  } else {
    c->GPRx = -1;
  }
}

void do_syscall(Context *c) {
  uintptr_t a[4];
  a[0] = c->GPR1;
  
  switch (a[0]) {
    case SYS_exit:
      halt(c->GPR2);
      break;
    case SYS_yield:
      c->GPRx = 0;
      yield();
      break;
    case SYS_write:
      do_write(c);
      break;
    default: panic("Unhandled syscall ID = %d", a[0]);
  }
}
