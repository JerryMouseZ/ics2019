#include "klib.h"
#include <stdarg.h>

#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)

char buffer[10240] = {0};

int printf(const char *fmt, ...)
{
  memset(buffer, 0, 10240);
  va_list ap;
  va_start(ap, fmt);
  int n = vsprintf(buffer, fmt, ap);
  va_end(ap);
  int i = 0;
  while (buffer[i] != 0)
  {
    _putc(buffer[i]);
    i++;
  }
  return n;
}

char mm[] = "0123456789abcdef";

void numtoStr(char *out, int num, int base, int width, char padc)
{
  if (num >= base)
  {
    numtoStr(out, num / base, base, width - 1, padc);
  }
  else
  {
    int len = strlen(out);
    while (--width > 0)
    {
      out[len++] = padc;
    }
    out[len] = 0;
  }
  int len = strlen(out);
  out[len++] = mm[num % base];
  out[len] = 0;
}

int vsprintf(char *out, const char *fmt, va_list ap)
{
  register const char *p;
  register int ch;
  unsigned long long num;
  int base, lflag, width, precision, altflag;
  char padc;
  //str len
  int len = 0;

  while (1)
  {
    while ((ch = *(unsigned char *)fmt++) != '%')
    {
      if (ch == '\0')
        return len;
      out[len++] = ch;
    }

    // Process a %-escape sequence
    padc = ' ';
    width = -1;
    precision = -1;
    lflag = 0;
    altflag = 0;
  reswitch:
    switch (ch = *(unsigned char *)fmt++)
    {

    // flag to pad on the right
    case '-':
      padc = '-';
      goto reswitch;

    // flag to pad with 0's instead of spaces
    case '0':
      padc = '0';
      goto reswitch;

    // width field
    case '1':
    case '2':
    case '3':
    case '4':
    case '5':
    case '6':
    case '7':
    case '8':
    case '9':
      for (precision = 0;; ++fmt)
      {
        precision = precision * 10 + ch - '0';
        ch = *fmt;
        if (ch < '0' || ch > '9')
          break;
      }
      goto process_precision;

    case '*':
      precision = va_arg(ap, int);
      goto process_precision;

    case '.':
      if (width < 0)
        width = 0;
      goto reswitch;

    case '#':
      altflag = 1;
      goto reswitch;

    process_precision:
      if (width < 0)
        width = precision, precision = -1;
      goto reswitch;

    // long flag (doubled for long long)
    case 'l':
      lflag++;
      goto reswitch;

    // character
    case 'c':
      out[len++] = va_arg(ap, int);
      // putch(va_arg(ap, int), putdat);
      break;

    // string
    case 's':
      if ((p = va_arg(ap, char *)) == NULL)
        p = "(null)";
      if (width > 0 && padc != '-')
        for (width -= strnlen(p, precision); width > 0; width--)
        {
          out[len] = 0;
          strcat(out, p);
          len += strlen(p);
        }
      for (; (ch = *p++) != '\0' && (precision < 0 || --precision >= 0); width--)
        if (altflag && (ch < ' ' || ch > '~'))
          out[len++] = '?';
        else
          out[len++] = ch;
      for (; width > 0; width--)
        out[len++] = ' ';
      break;

    // (signed) decimal
    case 'd':
      num = va_arg(ap, int);
      if ((long long)num < 0)
      {
        out[len++] = '-';
        num = -(long long)num;
      }
      base = 10;
      goto number;

    // unsigned decimal
    case 'u':
      num = va_arg(ap, unsigned);
      base = 10;
      goto number;

    // (unsigned) octal
    case 'o':
      // Replace this with your code.
      num = va_arg(ap, unsigned);
      base = 8;
      goto number;
      break;

    // pointer
    case 'p':
      out[len++] = '0';
      out[len++] = 'x';
      num = (unsigned long long)va_arg(ap, void *);
      base = 16;
      goto number;

    // (unsigned) hexadecimal
    case 'x':
      num = va_arg(ap, long);
      base = 16;
      goto number;
    number:
      out[len] = 0;
      char temp[256] = {0};
      numtoStr(temp, num, base, width, padc);
      strcat(out, temp);
      len += strlen(temp);
      break;
      // escaped '%' character

    case '%':
      out[len++] = ch;
      break;
    // unrecognized escape sequence - just print it literally
    default:
      out[len++] = '%';
      for (fmt--; fmt[-1] != '%'; fmt--)
        ;
      /* do nothing */;
      break;
    }
  }
  return len;
}

int sprintf(char *out, const char *fmt, ...)
{
  char *dd = out;
  while (*dd != 0)
  {
    *dd = 0;
    dd++;
  }
  va_list ap;
  va_start(ap, fmt);
  //vsprintf
  int n = vsprintf(out, fmt, ap);
  va_end(ap);
  return n;
}

int snprintf(char *out, size_t n, const char *fmt, ...)
{
  return 0;
}

#endif
