#include "cc.h"
#include "cpu/exec.h"

make_EHelper(test)
{
  // TODO();//比较但不修改寄存器，仅修改标志位
  rtl_and(&t0, &id_dest->val, &id_src->val);
  rtl_update_ZFSF(&t0, id_dest->width);
  //CF OF 0
  uint32_t dd = 0;
  rtl_set_CF(&dd);
  rtl_set_OF(&dd);
  print_asm_template2(test);
}

make_EHelper(and)
{
  // TODO();
  // dest = dest && src
  t0 = id_dest->val;
  t1 = id_src->val;
  rtl_and(&s0, &t0, &t1);
  rtl_update_ZFSF(&s0, id_dest->width);
  operand_write(id_dest, &s0);
  uint32_t dd = 0;
  rtl_set_OF(&dd);
  rtl_set_CF(&dd);

  print_asm_template2(and);
}

make_EHelper(xor)
{
  // TODO();
  t0 = id_dest->val;
  t1 = id_src->val;
  rtl_xor(&s0, &t0, &t1);
  operand_write(id_dest, &s0);
  rtl_update_ZFSF(&s0, id_dest->width);
  uint32_t dd = 0;
  rtl_set_CF(&dd);
  rtl_set_OF(&dd);
  print_asm_template2(xor);
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
  // TODO();
  // unnecessary to update CF and OF in NEMU
  if (id_dest->width == 1)
  {
    id_dest->val = (int8_t)id_dest->val;
  }
  else if (id_dest->width == 2)
  {
    id_dest->val = (int16_t)id_dest->val;
  }
  rtl_update_ZFSF(&s0, id_dest->width);
  operand_write(id_dest, &s0);
  print_asm_template2(sar);
}

make_EHelper(shl)
{
  // TODO();
  // unnecessary to update CF and OF in NEMU
  rtl_shl(&s0, &id_dest->val, &id_src->val);
  rtl_update_ZFSF(&s0, id_dest->width);
  operand_write(id_dest, &s0);
  print_asm_template2(shl);
}

make_EHelper(shr)
{
  // TODO();
  // unnecessary to update CF and OF in NEMU
  rtl_shr(&s0, &id_dest->val, &id_src->val);
  rtl_update_ZFSF(&s0, id_dest->width);
  operand_write(id_dest, &s0);
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
  // TODO();
  rtl_not(&s0, &id_dest->val);
  operand_write(id_dest, &s0);
  print_asm_template1(not);
}

make_EHelper(rol)
{
  switch (id_dest->width)
  {
  case 1:
    rtl_li(&s0, 0x80);
    break;
  case 2:
    rtl_li(&s0, 0x8000);
    break;
  case 4:
    rtl_li(&s0, 0x80000000);
    break;
  default:
    panic("Not supported width");
  }
  while (id_src->val > 0)
  {
    rtl_li(&s1, (s0 & id_dest->val) ? 1 : 0);
    rtl_shli(&id_dest->val, &id_dest->val, 1);
    rtl_add(&id_dest->val, &id_dest->val, &s1);
    rtl_subi(&id_src->val, &id_src->val, 1);
  }
  // write the answer back
  operand_write(id_dest, &id_dest->val);

  // // update ZF, SF
  // rtl_update_ZFSF(&id_dest->val, id_dest->width);
}