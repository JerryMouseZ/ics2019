#include "cpu/exec.h"

make_EHelper(mov);
make_EHelper(movzx);
make_EHelper(movsx);
make_EHelper(operand_size);

make_EHelper(inv);
make_EHelper(nemu_trap);

make_EHelper(call);
make_EHelper(jne);
make_EHelper(push);
make_EHelper(pop);
make_EHelper(sub);
make_EHelper(add);
make_EHelper (xor);
make_EHelper(or);
make_EHelper(adc);
make_EHelper(sbb);
make_EHelper(and);
make_EHelper(cmp);
make_EHelper(ret);
make_EHelper(lea);
make_EHelper(nop);
make_EHelper(sete)
{
  rtl_set_ZF(&decinfo.src.reg);
}
make_EHelper(pusha);
make_EHelper(popa);
make_EHelper(leave);

make_EHelper(test);
make_EHelper(je);
make_EHelper(inc);
make_EHelper(dec);
make_EHelper(jcc);