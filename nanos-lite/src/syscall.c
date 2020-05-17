#include "syscall.h"
#include "common.h"

_Context *do_syscall(_Context *c)
{
  uintptr_t a[4];
  a[0] = c->GPR1;
  a[1] = c->GPR2;
  a[2] = c->GPR3;
  a[3] = c->GPR4;
  switch (a[0])
  {
  case SYS_exit:
    Log("id = 0 exit");
    _halt(a[1]);
    c->GPRx = 0;
    break;
  case SYS_write:
    if (a[1] == 1 || a[1] == 2)
    {
      char *buf = (char *)a[2];
      int len = (int)a[3];
      for (int i = 0; i < len; i++)
      {
        _putc(buf[i]);
      }
      c->GPRx = len;
    }
    break;
  case SYS_yield:
    // _yield();
    // Log("syscall yield");
    c->GPRx = 0;
    break;
  default:
    panic("Unhandled syscall ID = %d", a[0]);
  }

  return NULL;
}
