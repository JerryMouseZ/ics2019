#include "proc.h"
#include <elf.h>
#include <fs.h>

#ifdef __ISA_AM_NATIVE__
#define Elf_Ehdr Elf64_Ehdr
#define Elf_Phdr Elf64_Phdr
#else
#define Elf_Ehdr Elf32_Ehdr
#define Elf_Phdr Elf32_Phdr
#endif

size_t ramdisk_read(void *buf, size_t offset, size_t len);
size_t ramdisk_write(const void *buf, size_t offset, size_t len);
size_t get_ramdisk_size();
Elf_Phdr pHeaders[20];
static uintptr_t loader(PCB *pcb, const char *filename)
{
  //第一个文件位置在0x3000000
  int base = 0x3000000;
  //读取文件头
  Elf_Ehdr elfHeader;
  size_t len = ramdisk_read(&elfHeader, 0, sizeof(Elf_Ehdr));
  assert(len == sizeof(Elf_Ehdr));
  Log("read elfheader\n");
  // base += len;
  //读取段头
  printf("%lx\n", elfHeader.e_phoff);
  ramdisk_read(pHeaders, elfHeader.e_phoff, elfHeader.e_phentsize * elfHeader.e_phnum);
  Log("read phheader\n");
  printf("sections number %d\n", elfHeader.e_phnum);
  for (int i = 0; i < elfHeader.e_phnum; i++)
  {
    printf("pHeader type check\n");
    if (pHeaders[i].p_type != PT_LOAD)
      continue;
    printf("segment offset %x\n", pHeaders[i].p_offset);
    //将该段读取到制定的内存位置
    ramdisk_read(&pHeaders[i].p_vaddr, pHeaders[i].p_offset, pHeaders[i].p_filesz);
    printf("read a section to phsical area\n");
    //如果出现没对齐的情况把相应的内存区域清0
    if (pHeaders[i].p_filesz < pHeaders[i].p_memsz)
      memset((void *)(pHeaders[i].p_vaddr + pHeaders[i].p_filesz), 0, pHeaders[i].p_memsz - pHeaders[i].p_filesz);
  }
  return elfHeader.e_entry;
}

void naive_uload(PCB *pcb, const char *filename)
{
  uintptr_t entry = loader(pcb, filename);
  Log("Jump to entry = %x", entry);
  ((void (*)())entry)();
}

void context_kload(PCB *pcb, void *entry)
{
  _Area stack;
  stack.start = pcb->stack;
  stack.end = stack.start + sizeof(pcb->stack);

  pcb->cp = _kcontext(stack, entry, NULL);
}

void context_uload(PCB *pcb, const char *filename)
{
  uintptr_t entry = loader(pcb, filename);

  _Area stack;
  stack.start = pcb->stack;
  stack.end = stack.start + sizeof(pcb->stack);

  pcb->cp = _ucontext(&pcb->as, stack, stack, (void *)entry, NULL);
}
