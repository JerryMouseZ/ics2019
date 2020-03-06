#ifndef __WATCHPOINT_H__
#define __WATCHPOINT_H__

#include "common.h"

typedef struct watchpoint
{
  int NO;
  struct watchpoint *next;
  char args[64];
  /* TODO: Add more members if necessary */

} WP;

bool check_wp();
#endif
