#include "memory.h"
#include "proc.h"

static void *pf = NULL;

void *new_page(size_t nr_page)
{
  void *p = pf;
  pf += PGSIZE * nr_page;
  assert(pf < (void *)_heap.end);
  return p;
}

void free_page(void *p)
{
  panic("not implement yet");
}

/* The brk() system call handler. */
int mm_brk(uintptr_t new_brk)
{
  // Log("new_brk = %x max_brk = %x\n", new_brk, current->max_brk);
  if (new_brk > current->max_brk)
  {
    uint32_t va = (current->max_brk & 0xfffff000);
    while (va < (new_brk & 0xfffff000))
    {
      void *pa = new_page(1);
      // printf("[brk] map %x to %x\n", va, pa);
      _map(&current->as, (void *)va, pa, 0);
      va += PGSIZE;
    }
    current->max_brk = new_brk;
  }
  return 0;
}

void init_mm()
{
  pf = (void *)PGROUNDUP((uintptr_t)_heap.start);
  Log("free physical pages starting from %p", pf);

  _vme_init(new_page, free_page);
}
