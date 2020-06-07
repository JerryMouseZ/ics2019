#include "rtl/rtl.h"

#define IDT_SIZE 8

void raise_intr(uint32_t NO, vaddr_t ret_addr)
{
  /* TODO: Trigger an interrupt/exception with ``NO''.
   * That is, use ``NO'' to index the IDT.
   */
  //保存当前环境
  rtl_push(&cpu.eflag);
  rtl_push(&cpu.cs);
  rtl_push(&ret_addr);
  cpu.eflags.IF = 0;
  s0 = IDTR(base);
  t0 = vaddr_read(s0 + NO * IDT_SIZE, 2);
  t1 = vaddr_read(s0 + NO * IDT_SIZE - 2, 2);
  s1 = (t1 << 16) + t0; // high bits ## low bits
  rtl_j(s1);
}

#define IRQ_TIMER 32 // for x86

bool isa_query_intr(void)
{
  if (cpu.INTR)
  {
    cpu.INTR = false;
    raise_intr(IRQ_TIMER, cpu.pc);
    return true;
  }
  return false;
}
