#include "klib.h"

#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)

size_t strlen(const char *s)
{
  int n = 0;
  while (s[n] != 0)
  {
    n++;
  }
  return n;
}

size_t strnlen(const char *s, size_t n)
{
  int i = 0;
  while (i < n && s[i] != 0)
  {
    i++;
  }
  return i;
}

char *strcpy(char *dst, const char *src)
{

  size_t i;
  for (i = 0; src[i] != '\0'; i++)
    dst[i] = src[i];
  return dst;
}

char *strncpy(char *dest, const char *src, size_t n)
{
  size_t i;

  for (i = 0; i < n && src[i] != '\0'; i++)
    dest[i] = src[i];
  for (; i < n; i++)
    dest[i] = '\0';

  return dest;
}

char *strcat(char *dest, const char *src)
{
  size_t dest_len = strlen(dest);
  strcpy(dest + dest_len, src);
  return dest;
}

int strcmp(const char *s1, const char *s2)
{
  int i = 0;
  while (s1[i] != 0 && s1[i] == s2[i])
  {
    i++;
  }
  return s1[i] - s2[i];
}

int strncmp(const char *s1, const char *s2, size_t n)
{
  return memcmp(s1, s2, n);
}

void* memset(void* v,int c,size_t n) {
  assert(v != NULL);
  uint8_t byte = (uint8_t) c;
  for (int i = 0; i < n; i++) {
    ((uint8_t*)v)[i] = byte;
  }
  return v;
}

void *memcpy(void *out, const void *in, size_t n)
{
  //这里如果out的值比in的大而且二者有重叠，则后面的部分就会是错的
  for (int i = 0; i < n; i++)
  {
    ((char *)out)[i] = ((char *)in)[i];
  }
  return out;
}

void *memmove(void *out, void *in, size_t n)
{
  // if (in + n < out || out + n < in)
  // {
  //   //先弄一份拷贝
  //   int more = 0;
  //   if (out > in)
  //   {
  //     more = out - in;
  //     char *temp = malloc(more);
  //     memcpy(temp, out - more, more);
  //     memcpy(out, in, n - more);
  //     memcpy(out, temp, more);
  //   }
  //   else
  //   {
  //     more = in - out;
  //     char *temp = malloc(more);
  //     memcpy(temp, in - more, more);
  //     memcpy(out, in, n - more);
  //     memcpy(out, temp, more);
  //   }
  // }
  // else
  // {
  //   memcpy(out, in, n);
  // }
  return out;
}

int memcmp(const void *s1, const void *s2, size_t n)
{
  for (int i = 0; i < n; i++)
  {
    if (((char *)s1)[i] == ((char *)s2)[i])
      continue;
    return ((char *)s1)[i] - ((char *)s2)[i];
  }
  return 0;
  // return 0;
}

#endif
