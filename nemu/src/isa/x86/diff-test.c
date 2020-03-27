#include "monitor/diff-test.h"
#include "nemu.h"

bool isa_difftest_checkregs(CPU_state *ref_r, vaddr_t pc)
{
  for (int i = R_EAX; i <= R_EDI; i++)
  {
    if (reg_l(i) != ref_r->gpr[i]._32)
    {
      printf("different at: %s\n", reg_name(i, 4));
      return false;
    }
  }
  if (cpu.pc != ref_r->pc)
  {
    printf("different at pc\n");
    return false;
  }
  // if(cpu.eflag != ref_r->eflag)
  // {
  //   printf("different at eflag\n");
  //   return false;
  // }
  return true;
}

void isa_difftest_attach(void)
{
}
