#include "common.h"

_Context *do_syscall(_Context *c);
static _Context *do_event(_Event e, _Context *c)
{
  switch (e.event)
  {
  case _EVENT_YIELD:
    printf("yield event\n");
    break;
  case _EVENT_SYSCALL:
    Log("syscall");
    do_syscall(c);
    Log("syscall done");
    break;
  default:
    panic("Unhandled event ID = %d", e.event);
  }

  return NULL;
}

void init_irq(void)
{
  Log("Initializing interrupt/exception handler...");
  _cte_init(do_event);
}
