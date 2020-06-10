#include "nemu.h"
#include <stdlib.h>
#include <time.h>

const char *regsl[] = {"eax", "ecx", "edx", "ebx", "esp", "ebp", "esi", "edi"};
const char *regsw[] = {"ax", "cx", "dx", "bx", "sp", "bp", "si", "di"};
const char *regsb[] = {"al", "cl", "dl", "bl", "ah", "ch", "dh", "bh"};

void reg_test()
{
  srand(time(0));
  uint32_t sample[8];
  uint32_t pc_sample = rand();
  cpu.pc = pc_sample;

  int i;
  for (i = R_EAX; i <= R_EDI; i++)
  {
    sample[i] = rand();
    reg_l(i) = sample[i];
    assert(reg_w(i) == (sample[i] & 0xffff));
  }

  assert(reg_b(R_AL) == (sample[R_EAX] & 0xff));
  assert(reg_b(R_AH) == ((sample[R_EAX] >> 8) & 0xff));
  assert(reg_b(R_BL) == (sample[R_EBX] & 0xff));
  assert(reg_b(R_BH) == ((sample[R_EBX] >> 8) & 0xff));
  assert(reg_b(R_CL) == (sample[R_ECX] & 0xff));
  assert(reg_b(R_CH) == ((sample[R_ECX] >> 8) & 0xff));
  assert(reg_b(R_DL) == (sample[R_EDX] & 0xff));
  assert(reg_b(R_DH) == ((sample[R_EDX] >> 8) & 0xff));

  assert(sample[R_EAX] == cpu.eax);
  assert(sample[R_ECX] == cpu.ecx);
  assert(sample[R_EDX] == cpu.edx);
  assert(sample[R_EBX] == cpu.ebx);
  assert(sample[R_ESP] == cpu.esp);
  assert(sample[R_EBP] == cpu.ebp);
  assert(sample[R_ESI] == cpu.esi);
  assert(sample[R_EDI] == cpu.edi);

  assert(pc_sample == cpu.pc);
}

void isa_reg_display()
{
  for (int i = R_EAX; i <= R_EDI; i++)
  {
    printf("%s 0x%08x\n", reg_name(i, 4), cpu.gpr[i]._32);
  }
  printf("%s 0x%08x\n", "eip", cpu.pc);
  printf("%s 0x%08x\n", "eflag", cpu.eflag);
}

void reg_display(CPU_state *r_ref)
{
  for (int i = R_EAX; i <= R_EDI; i++)
  {
    printf("%s 0x%08x\n", reg_name(i, 4), r_ref->gpr[i]._32);
  }
  printf("%s 0x%08x\n", "eip", r_ref->pc);
  printf("%s 0x%08x\n", "eflag", r_ref->eflag);
}

uint32_t isa_reg_str2val(const char *s, bool *success)
{
  return 0;
}

int isa_save_cpu_state(FILE *p)
{
  // save 8 basic registers
  for (int i = R_EAX; i <= R_EDI; i++)
  {
    fwrite(&cpu.gpr[i]._32, sizeof(rtlreg_t), 1, p);
  }
  // save eip
  fwrite(&cpu.pc, sizeof(vaddr_t), 1, p);

  // save flag registers
  fwrite(&cpu.eflag, sizeof(rtlreg_t), 1, p);

  // save CS
  fwrite(&cpu.cs, sizeof(rtlreg_t), 1, p);

  // save IDTR
  fwrite(&cpu.IDTR.limit, sizeof(unsigned short), 1, p);
  fwrite(&cpu.IDTR.base, sizeof(rtlreg_t), 1, p);
  return 0;
}

int isa_load_cpu_state(FILE *p)
{
  int ret;
  // load 8 basic registers
  for (int i = 0; i < 8; i++)
  {
    ret = fread(&cpu.gpr[i]._32, sizeof(rtlreg_t), 1, p);
    if (ret == -1)
      return ret;
  }
  // load eip
  ret = fread(&cpu.pc, sizeof(vaddr_t), 1, p);
  if (ret == -1)
    return ret;
  // load flag registers
  ret = fread(&cpu.eflag, sizeof(rtlreg_t), 1, p);
  if (ret == -1)
    return ret;
  // load CS
  ret = fread(&cpu.cs, sizeof(rtlreg_t), 1, p);
  if (ret == -1)
    return ret;
  // load IDTR
  ret = fread(&cpu.IDTR.limit, sizeof(unsigned short), 1, p);
  ret = fread(&cpu.IDTR.base, sizeof(rtlreg_t), 1, p);
  return ret;
}