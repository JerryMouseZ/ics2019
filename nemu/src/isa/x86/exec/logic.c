#include "cpu/exec.h"
#include "cc.h"

make_EHelper(test)
{
  TODO();

  print_asm_template2(test);
}

make_EHelper(and)
{
  // TODO();
  printf("esp : 0x%x\n", cpu.esp);
  t0 = id_dest->val;
  t1 = id_src->val;
  rtl_and(&s0, &t0, &t1);
  printf("esp : 0x%x\n", cpu.esp);
  // operand_write(id_dest, &id_dest->val);
  printf("esp : 0x%x\n", cpu.esp);
  rtl_update_ZFSF(&id_dest->val, id_dest->width);
  printf("esp : 0x%x\n", cpu.esp);
  print_asm_template2(and);
}

make_EHelper (xor)
{
  // TODO();
  rtl_xor(&id_dest->val, &id_dest->val, &id_src->val);
  // operand_write(id_dest, &id_dest->val);
  rtl_update_ZFSF(&id_dest->val, id_dest->width);
  print_asm_template2 (xor);
}

make_EHelper(or)
{
  //TODO();
  rtl_or(&id_dest->val,&id_dest->val, &id_src->val);
  // operand_write(id_dest, &id_dest->val);
  rtl_update_ZFSF(&id_dest->val, id_dest->width);
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
