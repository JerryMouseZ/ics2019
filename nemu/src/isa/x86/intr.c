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
  s0 = IDTR(base);
  t0 = vaddr_read(s0 + NO * IDT_SIZE, 2);
  t1 = vaddr_read(s0 + NO * IDT_SIZE - 2, 2);
  s1 = (t1 << 16) + t0; // high bits ## low bits
  rtl_j(s1);
}

bool isa_query_intr(void)
{
  return false;
}
