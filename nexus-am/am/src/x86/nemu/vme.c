#include <am.h>
#include <nemu.h>
#include <x86.h>

#define PG_ALIGN __attribute((aligned(PGSIZE)))

static PDE kpdirs[NR_PDE] PG_ALIGN = {};
static PTE kptabs[(PMEM_SIZE + MMIO_SIZE) / PGSIZE] PG_ALIGN = {};
static void *(*pgalloc_usr)(size_t) = NULL;
static void (*pgfree_usr)(void *) = NULL;
static int vme_enable = 0;

static _Area segments[] = { // Kernel memory mappings
    {.start = (void *)0, .end = (void *)PMEM_SIZE},
    {.start = (void *)MMIO_BASE, .end = (void *)(MMIO_BASE + MMIO_SIZE)}};

#define NR_KSEG_MAP (sizeof(segments) / sizeof(segments[0]))

int _vme_init(void *(*pgalloc_f)(size_t), void (*pgfree_f)(void *))
{
  pgalloc_usr = pgalloc_f;
  pgfree_usr = pgfree_f;

  int i;

  // make all PDEs invalid
  for (i = 0; i < NR_PDE; i++)
  {
    kpdirs[i] = 0;
  }

  PTE *ptab = kptabs;
  for (i = 0; i < NR_KSEG_MAP; i++)
  { //对段循环
    uint32_t pdir_idx = (uintptr_t)segments[i].start / (PGSIZE * NR_PTE);
    uint32_t pdir_idx_end = (uintptr_t)segments[i].end / (PGSIZE * NR_PTE);
    for (; pdir_idx < pdir_idx_end; pdir_idx++)
    {
      // fill PDE
      kpdirs[pdir_idx] = (uintptr_t)ptab | PTE_P;

      // fill PTE
      PTE pte = PGADDR(pdir_idx, 0, 0) | PTE_P;
      PTE pte_end = PGADDR(pdir_idx + 1, 0, 0) | PTE_P;
      for (; pte < pte_end; pte += PGSIZE)
      {
        *ptab = pte;
        ptab++;
      }
    }
  }
  set_cr3(kpdirs);
  set_cr0(get_cr0() | CR0_PG);
  vme_enable = 1;
  return 0;
}

int _protect(_AddressSpace *as)
{
  PDE *updir = (PDE *)(pgalloc_usr(1));
  as->ptr = updir;
  // map kernel space
  for (int i = 0; i < NR_PDE; i++)
  {
    updir[i] = kpdirs[i];
  }

  return 0;
}

void _unprotect(_AddressSpace *as)
{
  pgfree_usr(as->ptr);
  as->ptr = as->area.start = as->area.end = NULL;
}

static _AddressSpace *cur_as = NULL;
void __am_get_cur_as(_Context *c)
{
  c->as = cur_as;
}

void __am_switch(_Context *c)
{
  if (vme_enable)
  {
    set_cr3(c->as->ptr);
    cur_as = c->as;
  }
}

int _map(_AddressSpace *as, void *va, void *pa, int prot)
{
  //把pa映射到va的位置上
  uint32_t *ptr = (uint32_t *)as->ptr;
  uint32_t shift = (uintptr_t)va >> 22;
  uintptr_t tr = ptr[shift];
  if (tr == kpdirs[shift])
  {
    PTE *uptable = (PTE *)(pgalloc_usr(1));
    ptr[shift] = (uintptr_t)uptable | PTE_P;
  }
  tr = ptr[shift];
  shift = (((uintptr_t)va) & 0x003ff000) >> 12;
  uint32_t *pgr = (uint32_t *)(tr & 0xfffff000);
  pgr[shift] = (uintptr_t)pa | PTE_P;
  return 0;
}

/*
|               |
+---------------+ <---- ustack.end
|  stack frame  |
|   of _start() |
+---------------+
|               |
|    context    |
|               |
+---------------+ <--+
|               |    |
|               |    |
|               |    |
|               |    |
+---------------+    |
|       cp      | ---+
+---------------+ <---- ustack.start
|               |
*/
_Context *_ucontext(_AddressSpace *as, _Area ustack, _Area kstack, void *entry, void *args)
{
  //_protect分配页目录表
  printf("alloc pgdir : 0x%x\n", as->ptr);
  _Context *c = (_Context *)(ustack.end - 4 * sizeof(uintptr_t) - sizeof(_Context));
  *(uintptr_t *)(ustack.end - 3 * sizeof(uintptr_t)) = args;
  memset(c, 0, sizeof(_Context));
  // printf("u addr : %x\n", entry);
  c->as = as;
  c->eip = entry;
  c->eflags = 2;
  c->cs = 8;
  return c;
}
