#include "proc.h"

#define MAX_NR_PROC 4

static PCB pcb[MAX_NR_PROC] __attribute__((used)) = {};
static PCB pcb_boot = {};
PCB *current = NULL;

void switch_boot_pcb()
{
  current = &pcb_boot;
}

void hello_fun(void *arg)
{
  int j = 1;
  while (1)
  {
    Log("Hello World from Nanos-lite for the %dth time!", j);
    j++;
    _yield();
  }
}

void naive_uload(PCB *pcb, const char *filename);
void context_uload(PCB *pcb, const char *filename);
void context_kload(PCB *pcb, void *entry);
void init_proc()
{
  pcb[0].priority = 1;
  pcb[1].priority = 1000;
  context_uload(&pcb[0], "/bin/hello");
  context_uload(&pcb[1], "/bin/hello");
  switch_boot_pcb();
}

_Context *schedule(_Context *prev)
{
  current->cp = prev;
  // current->counter++;
  // if (current->counter > current->priority)
  // {
  //   current->counter = 0;
  current = (current == &pcb[0] ? &pcb[1] : &pcb[0]);
  // printf("switch to %x\n", current->as.ptr);
  // }
  return current->cp;
}
