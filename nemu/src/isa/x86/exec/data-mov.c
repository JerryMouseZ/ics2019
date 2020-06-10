#include "cpu/exec.h"

make_EHelper(mov)
{
  operand_write(id_dest, &id_src->val);
  print_asm_template2(mov);
}

make_EHelper(push)
{
  rtl_push(&id_dest->val);
  // Log("0x%x addr: 0x%x v: 0x%x", cpu.pc, cpu.esp, id_dest->val);
  print_asm_template1(push);
}

make_EHelper(pop)
{
  rtl_pop(&id_dest->val);

  // store data to the reg
  rtl_sr(id_dest->reg, &id_dest->val, 4);
  print_asm_template1(pop);
}

make_EHelper(pusha)
{
  rtl_li(&s1, reg_l(R_ESP));
  for (int i = R_EAX; i <= R_EDI; i++)
  {
    if (i == R_ESP)
    {
      rtl_push(&s1);
    }
    else
    {
      rtl_li(&s0, reg_l(i));
      rtl_push(&s0);
    }
  }
  print_asm("pusha");
}

make_EHelper(popa)
{
  for (int i = R_EDI; i >= R_EAX; i--)
  {
    rtl_pop(&s0);
    if (i == R_ESP)
      continue; // throwaway
    rtl_sr(i, &s0, 4);
  }
  print_asm("popa");
}

make_EHelper(leave)
{
  // set esp to ebp
  cpu.esp = cpu.ebp;
  // pop ebp
  rtl_pop(&cpu.ebp);
  print_asm("leave");
}

make_EHelper(cltd)
{
  if (decinfo.isa.is_operand_size_16)
  {
    if ((int16_t)reg_w(R_AX) < 0)
    {
      rtl_li(&s0, 0xffff);
    }
    else
    {
      rtl_li(&s0, 0x0);
    }
    rtl_sr(R_DX, &s0, 2);
  }
  else
  {
    if ((int32_t)reg_l(R_EAX) < 0)
    {
      rtl_li(&s0, 0xffffffff);
    }
    else
    {
      rtl_li(&s0, 0x0);
    }
    rtl_sr(R_EDX, &s0, 4);
  }

  print_asm(decinfo.isa.is_operand_size_16 ? "cwtl" : "cltd");
}

make_EHelper(cwtl)
{
  if (decinfo.isa.is_operand_size_16)
  {
    rtl_shli(&reg_l(R_EAX), &reg_l(R_EAX), 24);
    rtl_sari(&reg_l(R_EAX), &reg_l(R_EAX), 8);
    rtl_shri(&reg_l(R_EAX), &reg_l(R_EAX), 16);
  }
  else
  {
    rtl_sext(&reg_l(R_EAX), &reg_l(R_EAX), 2);
  }
  print_asm(decinfo.isa.is_operand_size_16 ? "cbtw" : "cwtl");
}

make_EHelper(movsx)
{
  id_dest->width = decinfo.isa.is_operand_size_16 ? 2 : 4;
  rtl_sext(&s0, &id_src->val, id_src->width);
  operand_write(id_dest, &s0);
  print_asm_template2(movsx);
}

make_EHelper(movzx)
{
  id_dest->width = decinfo.isa.is_operand_size_16 ? 2 : 4;
  operand_write(id_dest, &id_src->val);
  print_asm_template2(movzx);
}

make_EHelper(lea)
{
  operand_write(id_dest, &id_src->addr);
  print_asm_template2(lea);
}
