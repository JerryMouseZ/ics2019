#include "syscall.h"
#include "common.h"
#include "fs.h"
#include "proc.h"

extern char _end;

int mm_brk(uintptr_t new_brk);
int _execve(const char *path, char *const argv[], char *const envp[]);

_Context *do_syscall(_Context *c)
{
  uintptr_t a[4];
  a[0] = c->GPR1;
  a[1] = c->GPR2;
  a[2] = c->GPR3;
  a[3] = c->GPR4;
  // Log("System call: %d", a[0]);
  switch (a[0])
  {
  case SYS_exit:
    _halt(a[1]);
    // c->GPRx = _execve("/bin/init", (char **const)a[2], (char **const)a[3]);
    // c->GPRx = 0;
    break;
  case SYS_yield:
    _yield();
    c->GPRx = 0;
    break;
  case SYS_open:
    c->GPRx = fs_open((char *)a[1], a[2], a[3]);
    break;
  case SYS_read:
    c->GPRx = fs_read(a[1], (void *)a[2], a[3]);
    break;
  case SYS_write:
    // if (a[1] == 1 || a[1] == 2)
    //   c->GPRx = _write(0, (void *)a[2], a[3]);
    // else
    c->GPRx = fs_write(a[1], (void *)a[2], a[3]);
    break;
  case SYS_kill:
  case SYS_getpid:
  case SYS_close:
    break;
  case SYS_lseek:
    c->GPRx = fs_lseek(a[1], a[2], a[3]);
    break;
  case SYS_brk:
    c->GPRx = mm_brk(a[1]);
    break;
  case SYS_fstat:
  case SYS_time:
  case SYS_signal:
  case SYS_execve:
    c->GPRx = _execve((char *)a[1], (char **const)a[2], (char **const)a[3]);
    break;
  case SYS_fork:
  case SYS_link:
  case SYS_unlink:
  case SYS_wait:
  case SYS_times:
  case SYS_gettimeofday:
  default:
    panic("Unhandled syscall ID = %d", a[0]);
  }
  return NULL;
}

void naive_uload(PCB *pcb, const char *filename);
void context_uload(PCB *pcb, const char *filename);

int _execve(const char *path, char *const argv[], char *const envp[])
{
  //here shold replace with context_uload
  // context_uload(&)
  // naive_uload(NULL, path);
  return 0;
}