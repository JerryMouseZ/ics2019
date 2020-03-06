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
    wp_pool[i].isuse = false;
  }
  wp_pool[NR_WP - 1].next = NULL;

  head = NULL;
  free_ = wp_pool;
}
/* TODO: Implement the functionality of watchpoint */

WP *new_wp()
{
  // 把free的头加到 head的前面
  if (free_ == NULL)
  {
    assert(0);
    return NULL;
  }
  WP *result = free_;
  free_ = free_->next;
  result->next = head;
  head = result;
  return result;
}

void free_wp(char *args)
{
  printf("free a node\n");
  WP *temp = head;
  WP *wp = NULL;
  if (strcpy(temp->args, args))
  {
    head = head->next;
  }
  else
  {
    while (temp != NULL)
    {
      /* code */
      if (strcpy(temp->next->args, args))
      {
        WP *dd = temp->next->next;
        temp->next = dd;
        break;
      }
      temp = temp->next;
    }
  }
  if (!wp)
  {
    wp->next = free_;
    free_ = wp;
  }
}

bool check_wp()
{
  WP *wp = head;
  while (wp != NULL)
  {
    bool success;
    int result = expr(wp->args, &success);
    printf("%d\n", result);
    if (success && result > 0)
    {
      printf("%d\n", result);
      return true;
    }
    wp = wp->next;
  }
  return false;
}
