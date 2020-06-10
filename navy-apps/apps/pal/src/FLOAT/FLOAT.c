#include "FLOAT.h"
#include <stdint.h>
#include <assert.h>

FLOAT F_mul_F(FLOAT a, FLOAT b)
{
  return (a >> 8) * (b >> 8);
}

FLOAT F_div_F(FLOAT a, FLOAT b)
{
  return (a / b) << 16;
}

FLOAT f2F(float a)
{
  /* You should figure out how to convert `a' into FLOAT without
   * introducing x87 floating point instructions. Else you can
   * not run this code in NEMU before implementing x87 floating
   * point instructions, which is contrary to our expectation.
   *
   * Hint: The bit representation of `a' is already on the
   * stack. How do you retrieve it to another variable without
   * performing arithmetic operations on it directly?
   */
  uint32_t sign = (unsigned)a >> 31;
  int exponent = ((unsigned)a) >> 23;
  assert(exponent < 16);
  int fraction = ((unsigned)a << 9) >> 9;
  int result = (fraction << exponent) << 16;
  if (sign)
    result = -result;
  return result;
}

FLOAT Fabs(FLOAT a)
{
  if (a < 0)
    return -a;
  return a;
}

/* Functions below are already implemented */

FLOAT Fsqrt(FLOAT x)
{
  FLOAT dt, t = int2F(2);

  do
  {
    dt = F_div_int((F_div_F(x, t) - t), 2);
    t += dt;
  } while (Fabs(dt) > f2F(1e-4));

  return t;
}

FLOAT Fpow(FLOAT x, FLOAT y)
{
  /* we only compute x^0.333 */
  FLOAT t2, dt, t = int2F(2);

  do
  {
    t2 = F_mul_F(t, t);
    dt = (F_div_F(x, t2) - t) / 3;
    t += dt;
  } while (Fabs(dt) > f2F(1e-4));

  return t;
}
