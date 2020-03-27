#ifndef __X86_RTL_H__
#define __X86_RTL_H__

#include "rtl/rtl.h"

/* RTL pseudo instructions */

static inline void rtl_lr(rtlreg_t *dest, int r, int width)
{
  switch (width)
  {
  case 4:
    rtl_mv(dest, &reg_l(r));
    return;
  case 1:
    rtl_host_lm(dest, &reg_b(r), 1);
    return;
  case 2:
    rtl_host_lm(dest, &reg_w(r), 2);
    return;
  default:
    assert(0);
  }
}

static inline void rtl_sr(int r, const rtlreg_t *src1, int width)
{
  switch (width)
  {
  case 4:
    rtl_mv(&reg_l(r), src1);
    return;
  case 1:
    rtl_host_sm(&reg_b(r), src1, 1);
    return;
  case 2:
    rtl_host_sm(&reg_w(r), src1, 2);
    return;
  default:
    assert(0);
  }
}

static inline void rtl_push(const rtlreg_t *src1)
{
  // esp <- esp - 4
  // M[esp] <- src1
  // printf("esp: 0x%x\n", cpu.esp);
  cpu.esp = cpu.esp - 4;
  vaddr_write(cpu.esp, *src1, 4);
}

static inline void rtl_pop(rtlreg_t *dest)
{
  // dest <- M[esp]
  // esp <- esp + 4
  // printf("esp: 0x%x\n", cpu.esp);
  *dest = vaddr_read(cpu.esp, 4);
  cpu.esp = cpu.esp + 4;
}

static inline void rtl_is_sub_overflow(rtlreg_t *dest,
                                       const rtlreg_t *res, const rtlreg_t *src1, const rtlreg_t *src2, int width)
{
  // dest <- is_overflow(src1 - src2)
  //同号相减不会溢出，正数减负数或者负数减正数
  if (*src1 >> (width * 8 - 1) && *src2 >> (width * 8 - 1))
  {
    *dest = 0;
  }
  else if (*src1 >> (width * 8 - 1) || *src2 >> (width * 8 - 1))
  {
    if (((*src1 >> (width * 8 - 1)) & 1) ^ (*res >> (width * 8 - 1)))
    {
      *dest = 1;
    }
    else
    {
      *dest = 0;
    }
  }
  else
  {
    *dest = 0;
  }
}

static inline void rtl_is_sub_carry(rtlreg_t *dest,
                                    const rtlreg_t *res, const rtlreg_t *src1)
{
  // dest <- is_carry(src1 - src2)
  if (*res > *src1)
    *dest = 1;
  else
  {
    *dest = 0;
  }
}

static inline void rtl_is_add_overflow(rtlreg_t *dest,
                                       const rtlreg_t *res, const rtlreg_t *src1, const rtlreg_t *src2, int width)
{
  //判断有符号数的
  // dest <- is_overflow(src1 + src2)
  //两个负数相加，结果变成正数或者两个整数相加，结果变成负数
  //　这里就当width为４吧
  //
  if (*src1 >> (width * 8 - 1) && *src2 >> (width * 8 - 1))
  {
    //两个操作数是负数，结果要变成正数
    if (!(*res >> (width * 8 - 1)))
      *dest = 1;
    else
    {
      *dest = 0;
    }
  }
  else if (!(*src1 >> (width * 8 - 1)) && !(*src2 >> (width * 8 - 1)))
  {
    if (*res >> (width * 8 - 1))
      *dest = 1;
    else
    {
      *dest = 0;
    }
  }
  else
  {
    *dest = 0;
  }
}

static inline void rtl_is_add_carry(rtlreg_t *dest,
                                    const rtlreg_t *res, const rtlreg_t *src1)
{
  // dest <- is_carry(src1 + src2)
  if (*res < *src1)
  {
    *dest = 1;
  }
  else
  {
    *dest = 0;
  }
}

#define make_rtl_setget_eflags(f)                             \
  static inline void concat(rtl_set_, f)(const rtlreg_t *src) \
  {                                                           \
    cpu.eflags.f = *src;                                      \
  }                                                           \
  static inline void concat(rtl_get_, f)(rtlreg_t * dest)     \
  {                                                           \
    *dest = cpu.eflags.f;                                     \
  }

make_rtl_setget_eflags(CF)
    make_rtl_setget_eflags(OF)
        make_rtl_setget_eflags(ZF)
            make_rtl_setget_eflags(SF)

                static inline void rtl_update_ZF(const rtlreg_t *result, int width)
{
  // eflags.ZF <- is_zero(result[width * 8 - 1 .. 0])
  uint32_t flags = 0;
  switch (width)
  {
  case 4:
    flags = 0xffffffff;
    break;
  case 2:
    flags = 0xffff;
    break;
  case 1:
    flags = 0xff;
    break;
  default:
    assert(0);
  }
  if (*result & flags)
  {
    cpu.eflags.ZF = 0;
  }
  else
  {
    cpu.eflags.ZF = 1;
  }
}

static inline void rtl_update_SF(const rtlreg_t *result, int width)
{
  // eflags.SF <- is_sign(result[width * 8 - 1 .. 0])
  if ((*result >> (width * 8 - 1)) & 1)
  {
    cpu.eflags.SF = 1;
  }
  else
  {
    cpu.eflags.SF = 0;
  }
}

static inline void rtl_update_ZFSF(const rtlreg_t *result, int width)
{
  rtl_update_ZF(result, width);
  rtl_update_SF(result, width);
}

#endif
