#include "nemu.h"

void dev_raise_intr()
{
    if (cpu.eflags.IF)
        cpu.INTR = true;
}
