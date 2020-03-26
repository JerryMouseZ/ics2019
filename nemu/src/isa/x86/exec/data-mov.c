#include "cpu/exec.h"

make_EHelper(mov)
{
  operand_write(id_dest, &id_src->val);
  print_asm_template2(mov);
}

make_EHelper(push)
{
  // TODO();
  rtl_push(&decinfo.dest.val);
  print_asm_template1(push);
}

make_EHelper(pop)
{
  // TODO();
  rtl_pop(&decinfo.dest.val);
  print_asm_template1(pop);
}

//60
make_EHelper(pusha)
{
  // TODO(); //压入所有寄存器,不需要操作数
  if (decinfo.isa.is_operand_size_16)
  {
    uint16_t temp = reg_l(R_SP);
    for (int i = R_AX; i <= R_DI; i++)
    {
      if (i != R_SP)
      {
        rtl_push(&reg_l(i));
      }
      else
      {
        rtl_push(&temp);
      }
    }
  }
  else
  {
    uint32_t temp = cpu.esp;
    for (int i = R_EAX; i <= R_EDI; i++)
    {
      if (i != R_ESP)
        rtl_push(&reg_l(i));
      else
      {
        rtl_push(&temp);
      }
    }
  }
  print_asm("pusha");
}

//61
make_EHelper(popa)
{
  // TODO();//弹出所有寄存器
  if (decinfo.isa.is_operand_size_16)
  {
    uint16_t temp = 0;
    for (int i = R_AX; i <= R_DI; i++)
    {
      if (i != R_SP)
      {
        rtl_pop(&reg_l(i));
      }
      else
      {
        rtl_pop(&temp);
      }
    }
  }
  else
  {
    uint32_t temp = 0;
    for (int i = R_EAX; i <= R_EDI; i++)
    {
      if (i != R_ESP)
        rtl_pop(&reg_l(i));
      else
      {
        rtl_pop(&temp);
      }
    }
  }
  print_asm("popa");
}

make_EHelper(leave)
{
  TODO();

  print_asm("leave");
}

make_EHelper(cltd)
{
  if (decinfo.isa.is_operand_size_16)
  {
    TODO();
  }
  else
  {
    TODO();
  }

  print_asm(decinfo.isa.is_operand_size_16 ? "cwtl" : "cltd");
}

make_EHelper(cwtl)
{
  if (decinfo.isa.is_operand_size_16)
  {
    TODO();
  }
  else
  {
    TODO();
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
  // id_dest->width = decinfo.isa.is_operand_size_16 ? 2 : 4;
  // operand_write(id_dest, &id_src->val);
  print_asm_template2(movzx);
}

make_EHelper(lea)
{
  operand_write(id_dest, &id_src->addr);
  print_asm_template2(lea);
}
