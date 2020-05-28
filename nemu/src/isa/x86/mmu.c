#include "nemu.h"

const uint32_t present = 1;
paddr_t page_translate(vaddr_t vaddr)
{
  if (cpu.CR0.PG) //如果开启了分页
  {
    //页表项的低12位是权限相关的
    uint32_t pde = (uint32_t *)cpu.cr3;
    //前10位查一级页表
    uint32_t offset1 = (vaddr >> 22), offset2 = (vaddr >> 12) & 0x3ff, offset3 = vaddr & PAGE_MASK;
    uint32_t pte = paddr_read(pde + offset1, 4);
    //pte是否有效
    assert(pte & 1);
    pte &= PAGE_MASK;
    uint32_t page_addr = paddr_read(pte + offset2, 4);
    //页面是否有效
    assert(page_addr & 1);
    page_addr &= PAGE_MASK;
    return page_addr + offset3;
  }
  else
  {
    return (paddr_t)vaddr;
  }
}

uint32_t isa_vaddr_read(vaddr_t addr, int len)
{
  //暂时不知道怎么测试虚拟地址越界
  if (0)
  {
    /* this is a special case, you can handle it later. */
    assert(0);
  }
  else
  {
    //需要考虑到横跨两个页的情况
    if (((addr + len - 1) & PAGE_MASK) != (addr & PAGE_MASK))
    {
      uint32_t temp = (addr + len - 1) & PAGE_MASK;
      int l1 = temp - addr, l2 = addr + len - temp;
      uint32_t paddr1 = page_translate(addr), paddr2 = page_translate(temp);
      return paddr_read(paddr1, l1) | (paddr_read(paddr2, l2) << l1);
    }
    paddr_t paddr = page_translate(addr);
    return paddr_read(paddr, len);
  }
  // return paddr_read(addr, len);
}

void isa_vaddr_write(vaddr_t addr, uint32_t data, int len)
{
  if (0)
  {
    assert(0);
  }
  else
  {
    if (((addr + len - 1) & PAGE_MASK) != (addr & PAGE_MASK))
    {
      uint32_t temp = (addr + len - 1) & PAGE_MASK;
      int l1 = temp - addr, l2 = addr + len - temp;
      uint32_t paddr1 = page_translate(addr), paddr2 = page_translate(temp);
      //写低位
      paddr_write(paddr1, (data << l2) >> l2, l1);
      //写高位
      paddr_write(paddr2, (data >> l1), l2);
      return;
    }
    paddr_t paddr = page_translate(addr);
    paddr_write(paddr, data, len);
  }
}
