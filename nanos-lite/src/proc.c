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

void init_proc()
{

  context_uload(&pcb[1], "/bin/init");
  context_kload(&pcb[0], (void *)hello_fun);
  switch_boot_pcb();

  // Log("Initializing processes...");
  // // load program here
  // Log("load text program");
  // naive_uload(NULL, "/bin/init");
}

_Context *schedule(_Context *prev)
{
  current->cp = prev;
  // current = &pcb[1];
  current = (current == &pcb[0] ? &pcb[1] : &pcb[0]);
  // printf("schedule to eip : %x\n", current->cp->eip);
  return current->cp;
}
