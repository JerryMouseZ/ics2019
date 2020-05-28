#include "include/isa/mmu.h"
#include "nemu.h"

const uint32_t present = 1;
paddr_t page_translate(vaddr_t vaddr, bool iswrite)
{
  if (cpu.CR0.PG) //如果开启了分页
  {
    //页表项的低12位是权限相关的
    uint32_t offset1 = (vaddr >> 22), offset2 = (vaddr >> 12) & 0x3ff, offset3 = vaddr & 0x3ff;
    //pde
    PDE pde;
    pde.val = paddr_read((cpu.cr3 & PAGE_MASK) + offset1, 4);
    assert(pde.present);
    pde.accessed = 1;
    //pte
    PTE pte;
    pte.val = paddr_read((pde.page_frame << 12) + offset2, 4);
    assert(pte.present);
    pte.accessed = 1;
    if (iswrite)
      pte.dirty = 1;

    uint32_t page_addr = (pte.page_frame << 12) + offset3;
    return (paddr_t)page_addr;
  }
  else
  {
    return (paddr_t)vaddr;
  }
}

uint32_t isa_vaddr_read(vaddr_t addr, int len)
{
  //需要考虑到横跨两个页的情况
  if (((addr + len - 1) & PAGE_MASK) != (addr & PAGE_MASK))
  {
    uint32_t temp = (addr + len - 1) & PAGE_MASK;
    int l1 = temp - addr, l2 = addr + len - temp;
    uint32_t paddr1 = page_translate(addr, 0), paddr2 = page_translate(temp, 0);
    return paddr_read(paddr1, l1) | (paddr_read(paddr2, l2) << l1);
  }
  paddr_t paddr = page_translate(addr, 0);
  return paddr_read(paddr, len);

  // return paddr_read(addr, len);
}

void isa_vaddr_write(vaddr_t addr, uint32_t data, int len)
{

  if (((addr + len - 1) & PAGE_MASK) != (addr & PAGE_MASK))
  {
    uint32_t temp = (addr + len - 1) & PAGE_MASK;
    int l1 = temp - addr, l2 = addr + len - temp;
    uint32_t paddr1 = page_translate(addr, 1), paddr2 = page_translate(temp, 1);
    //写低位
    paddr_write(paddr1, (data << l2) >> l2, l1);
    //写高位
    paddr_write(paddr2, (data >> l1), l2);
    return;
  }
  paddr_t paddr = page_translate(addr, 1);
  paddr_write(paddr, data, len);
}
