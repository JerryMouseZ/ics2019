#include "cc.h"
#include "cpu/exec.h"

make_EHelper(test)
{
  // TODO();//比较但不修改寄存器，仅修改标志位
  rtl_and(&t0, &id_dest->val, &id_src->val);
  rtl_msb(&s0, &t0, decinfo.width);
  rtl_set_SF(&s0);
  uint32_t dd = 1;
  if (s0 == 0)
  {
    rtl_set_ZF(&dd);
  }
  else
  {
    dd = 0;
    rtl_set_ZF(&dd);
  }
  //CF OF 0
  dd = 0;
  rtl_set_CF(&dd);
  rtl_set_OF(&dd);
  print_asm_template2(test);
}

make_EHelper(and)
{
  // TODO();
  // dest = dest && src
  printf("esp : 0x%x\n", cpu.esp);
  t0 = id_dest->val;
  t1 = id_src->val;
  rtl_andi(&s0, &t0, &t1);
  rtl_update_ZFSF(&s0, id_dest->width);
  printf("esp : 0x%x\n", cpu.esp);
  operand_write(id_dest, &s0);
  printf("esp : 0x%x\n", cpu.esp);
  uint32_t dd = 0;
  rtl_set_OF(&dd);
  rtl_set_CF(&dd);

  printf("esp : 0x%x\n", cpu.esp);
  print_asm_template2(and);
}

make_EHelper (xor)
{
  // TODO();
  t0 = id_dest->val;
  t1 = id_src->val;
  rtl_xori(&s0, &t0, &t1);
  operand_write(id_dest, &s0);
  rtl_update_ZFSF(&s0, id_dest->width);
  uint32_t dd = 0;
  rtl_set_CF(&dd);
  rtl_set_OF(&dd);
  print_asm_template2 (xor);
}

make_EHelper(or)
{
  //TODO();
  t0 = id_dest->val;
  t1 = id_src->val;
  rtl_or(&s0, &t0, &t1);
  operand_write(id_dest, &s0);
  rtl_update_ZFSF(&s0, id_dest->width);
  uint32_t dd = 0;
  rtl_set_CF(&dd);
  rtl_set_OF(&dd);
  print_asm_template2(or);
}

make_EHelper(sar)
{
  TODO();
  // unnecessary to update CF and OF in NEMU

  print_asm_template2(sar);
}

make_EHelper(shl)
{
  TODO();
  // unnecessary to update CF and OF in NEMU

  print_asm_template2(shl);
}

make_EHelper(shr)
{
  TODO();
  // unnecessary to update CF and OF in NEMU

  print_asm_template2(shr);
}

make_EHelper(setcc)
{
  uint32_t cc = decinfo.opcode & 0xf;

  rtl_setcc(&s0, cc);
  operand_write(id_dest, &s0);

  print_asm("set%s %s", get_cc_name(cc), id_dest->str);
}

make_EHelper(not)
{
  TODO();

  print_asm_template1(not);
}
