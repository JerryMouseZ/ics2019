#include "include/isa/mmu.h"
#include "nemu.h"

#define PAGE 0xfffff000

const uint32_t present = 1;
//translate肯定写错了
paddr_t page_translate(vaddr_t vaddr, bool iswrite)
{
  paddr_t dir = (vaddr >> 22) & 0x3ff;
  paddr_t page = (vaddr >> 12) & 0x3ff;
  paddr_t offset = vaddr & 0xfff;
  paddr_t paddr = vaddr;
  if (cpu.CR0.PG)
  {
    uint32_t page_directory_base = cpu.CR3.page_directory_base;
    uint32_t page_table = paddr_read((page_directory_base << 12) + (dir << 2), 4);
    if ((page_table & 1) == 0)
    {
      printf("[translating] %x\n", vaddr);
      assert(0);
    }
    uint32_t page_frame = paddr_read((page_table & 0xffffff000) + (page << 2), 4);
    if ((page_frame & 1) == 0)
    {
      printf("[translating] %x\n", vaddr);
      assert(0);
    }
    paddr = (page_frame & 0xffffff000) + offset;
  }
  return paddr;
}

uint32_t isa_vaddr_read(vaddr_t addr, int len)
{
  //需要考虑到横跨两个页的情况
  if (((addr + len - 1) & PAGE) != (addr & PAGE))
  {
    union {
      uint8_t bytes[4];
      uint32_t dword;
    } data = {0};
    uint32_t paddr;
    for (int i = 0; i < len; i++)
    {
      paddr = page_translate(addr + i, 0);
      data.bytes[i] = paddr_read(paddr, 1);
    }
    return data.dword;
  }
  paddr_t paddr = page_translate(addr, 0);
  return paddr_read(paddr, len);
}

void isa_vaddr_write(vaddr_t addr, uint32_t data, int len)
{

  if (((addr + len - 1) & PAGE) != (addr & PAGE))
  {
    uint32_t paddr = 0;
    for (int i = 0; i < len; i++)
    {
      paddr = page_translate(addr + i, true);
      paddr_write(paddr, data & 0xf, 1);
      data >>= 8;
    }
    return;
  }
  paddr_t paddr = page_translate(addr, 1);
  paddr_write(paddr, data, len);
}
