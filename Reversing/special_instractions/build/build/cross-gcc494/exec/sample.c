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

/*
 * Moxie machine code format
 * 
 * Form3: (branch instruction)
 *  5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * |1|1|  op   |     offset      |N|
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * 
 * Form2:
 *  5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * |1|0|op |  reg  |       v       |
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * 
 * Form2:
 *  5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * |0|0|    op     | regA  | regB  |
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 */

void set_random_seed(unsigned int seed)
{
  asm volatile (".byte 0x16"); /* Opcode: setrseed */
  asm volatile (".byte 0x20"); /* Oprand: r0 */
}

unsigned int get_random_value(unsigned int dummy)
{
  asm volatile (".byte 0x17"); /* Opcode: getrand */
  asm volatile (".byte 0x20"); /* Oprand: r0 */
  return dummy;
}

unsigned char randval[] = {
  0x3D, 0x05, 0xDC, 0x31, 0xD1, 0x8A, 0xAF, 0x29,
  0x96, 0xFA, 0xCB, 0x1B, 0x01, 0xEC, 0xE2, 0xF7,
  0x15, 0x70, 0x6C, 0xF4, 0x7E, 0xA1, 0x9E, 0x0E,
  0x01, 0xF9, 0xC2, 0x4C, 0xBA, 0xA0, 0xA1, 0x08,
  0x70, 0x24, 0x85, 0x8A, 0x4D, 0x2D, 0x3C, 0x02,
  0xFC, 0x6F, 0x20, 0xF0, 0xC7, 0xAD, 0x2F, 0x97,
  0x2B, 0xCC, 0xA3, 0x34, 0x23, 0x53, 0xC9, 0xB7,
  0x0C, 0x10, 0x6C, 0x0E, 0xFA, 0xF9, 0xA1, 0x9A,
};

#ifdef MAKE_QUESTION
unsigned char flag[] = "SECCON{MakeSpecialInstructions}";
#else
unsigned char flag[] = {
  0x6D, 0x72, 0xC3, 0xE2, 0xCF, 0x95, 0x54, 0x9D,
  0xB6, 0xAC, 0x03, 0x84, 0xC3, 0xC2, 0x35, 0x93,
  0xC3, 0xD7, 0x7C, 0xE2, 0xDD, 0xD4, 0xAC, 0x5E,
  0x99, 0xC9, 0xA5, 0x34, 0xDE, 0x06, 0x4E, 0
};
#endif

unsigned char *decode(unsigned char *str, unsigned char *key)
{
  int i;
  for (i = 0; str[i]; i++) {
    str[i] ^= (key[i] ^ get_random_value(0));
  }
  return str;
}

int main()
{
  int fd = 1;

  set_random_seed(2463534242U);

  puts(fd, "This program uses special instructions.\n\n");
  puts(fd, "SETRSEED: (Opcode:0x16)\n");
  puts(fd, "\tRegA -> SEED\n\n");
  puts(fd, "GETRAND: (Opcode:0x17)\n");
  puts(fd, "\txorshift32(SEED) -> SEED\n");
  puts(fd, "\tSEED -> RegA\n\n");

  puts(fd, (char *)decode(flag, randval));
  puts(fd, "\n");

  exit(0);
  return 0;
}
