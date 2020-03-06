#include "monitor/watchpoint.h"
#include "monitor/expr.h"

#define NR_WP 32

static WP wp_pool[NR_WP] = {};
static WP *head = NULL, *free_ = NULL;

void init_wp_pool()
{
  int i;
  for (i = 0; i < NR_WP; i++)
  {
    wp_pool[i].NO = i;
    wp_pool[i].next = &wp_pool[i + 1];
  }
  wp_pool[NR_WP - 1].next = NULL;

  head = NULL;
  free_ = wp_pool;
}
/* TODO: Implement the functionality of watchpoint */

WP *new_wp()
{
  return head;
}

void free_wp(WP *wp)
{
}

bool check_wp()
{
  WP *wp = head;
  while (wp != NULL)
  {
    bool success;
    int result = expr(wp->args, &success);
    if (success && result > 0)
      return true;
  }
  return false;
}
