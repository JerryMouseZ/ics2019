#include "common.h"

_Context *do_syscall(_Context *c);
static _Context *do_event(_Event e, _Context *c)
{
  _Context *result = NULL;
  switch (e.event)
  {
  case _EVENT_YIELD:
    printf("yield event\n");
    break;
  case _EVENT_SYSCALL:
    printf("event syscall\n");
    result = do_syscall(c);
    break;
  default:
    panic("Unhandled event ID = %d", e.event);
  }

  return result;
}

void init_irq(void)
{
  Log("Initializing interrupt/exception handler...");
  _cte_init(do_event);
}
