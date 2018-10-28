/* #define MAKE_QUESTION */

#include "syscall.h"

#ifdef USE_SYSCALL_WRAPPER
void __exit(int status)
{
  __r_exit(0, status);
}
int __read(int fd, void *buffer, int size)
{
  return __r_read(0, fd, buffer, size);
}
int __write(int fd, const void *buffer, int size)
{
  return __r_write(0, fd, buffer, size);
}
int __open(const char *path, int flags, int mode)
{
  return __r_open(0, path, flags, mode);
}
int __close(int fd)
{
  return __r_close(0, fd);
}
#endif

void exit(int status)
{
  __exit(status);
}

int write1(int fd, unsigned char c)
{
  return __write(fd, &c, 1);
}

int putchar(int fd, int c)
{
  write1(fd, c);
  return c;
}

int puts(int fd, char *str)
{
  for (; *str; str++)
    putchar(fd, *str);
  return 0;
}

#ifdef RSHIFT
/*
 * For architecture not to be able to use shift instruction for well,
 * execute shift operation without shift instruction.
 */
unsigned long rshift1(unsigned long value)
{
  unsigned long mask, bit = 1;
  unsigned long ret = 0;
  int i;

  for (i = 0; i < sizeof(unsigned long) * 8 - 1; i++) {
    mask = bit + bit; /* Use addition for no using shift and multiplication */
    if (value & mask)
      ret |= bit;
    bit = bit + bit;
  }

  return ret;
}

unsigned long RSHIFT(unsigned long value)
{
  int num = 4;
  for (; num > 0; num--)
    value = rshift1(value);
  return value;
}
#endif

int putxval(int fd, unsigned long value, int column)
{
  char buf[17];
  char *p;

  p = buf + sizeof(buf) - 1;
  *(p--) = '\0';

  if (!value && !column)
    column++;

  while (value || column) {
    *(p--) = "0123456789abcdef"[value & 0xf];
    value >>= 4;
    if (column) column--;
  }

  puts(fd, p + 1);

  return 0;
}

unsigned char randval[] = {
  0x1D, 0xAB, 0x1B, 0x0F, 0xA7, 0xD9, 0x1A, 0xB0,
  0x61, 0x7E, 0xB6, 0x48, 0xA4, 0x56, 0xCF, 0x7E,
  0x49, 0x05, 0xFD, 0x05, 0x9C, 0xF9, 0x54, 0x45,
  0xFA, 0x24, 0xC6, 0x1D, 0x68, 0xF2, 0x46, 0xCE,
  0xC1, 0xAD, 0xAB, 0x08, 0x24, 0x86, 0x9C, 0xF8,
  0x58, 0x65, 0x62, 0x88, 0x49, 0x22, 0x82, 0x11,
  0x29, 0x14, 0x63, 0x74, 0xAE, 0x28, 0xCE, 0x8C,
  0x79, 0x2D, 0xAB, 0x07, 0xBB, 0x75, 0x25, 0x9D,
};

#ifdef MAKE_QUESTION
unsigned char flag[] = "SECCON{UseTheSpecialDeviceFile}";
#else
unsigned char flag[] = {
  0xFE, 0x75, 0x88, 0xA9, 0x5A, 0xAA, 0x10, 0x52,
  0x9C, 0x6A, 0x67, 0xF4, 0x82, 0xBE, 0x21, 0x56,
  0x59, 0x0B, 0x97, 0x32, 0x21, 0x46, 0x93, 0xAE,
  0x40, 0x0D, 0x2E, 0x1F, 0x83, 0x43, 0x40, 0
};
#endif

unsigned long get_random_value(int fd)
{
  unsigned long value;
  __read(fd, &value, sizeof(value));
  return value;
}

unsigned char *decode(unsigned char *str, unsigned char *key, int fd)
{
  int i;
  for (i = 0; str[i]; i++) {
    str[i] ^= (key[i] ^ get_random_value(fd));
  }
  return str;
}

int main()
{
  int fd;
  unsigned long seed = 88172645463325252UL;

  fd = __open("/dev/xorshift64", 1, 0); /* O_WRONLY */
  __write(fd, &seed, sizeof(seed));
  __close(fd);

  fd = __open("/dev/xorshift64", 0, 0); /* O_RDONLY */
  puts(1, (char *)decode(flag, randval, fd));
  puts(1, "\n");
  __close(fd);

  exit(0);
  return 0;
}
