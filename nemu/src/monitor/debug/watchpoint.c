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

void free_wp(WP *wp)
{
  if (!wp->isuse)
  {
    printf("error free number\n");
    return 0;
  }
  WP *temp = head;
  if (temp == wp)
  {
    head = head->next;
  }
  else
  {
    while (temp != NULL)
    {
      /* code */
      if (temp->next == wp)
      {
        WP *dd = temp->next->next;
        temp->next = dd;
        break;
      }
      temp = temp->next;
    }
  }
  wp->isuse = false;
  wp->next = free_;
  free_ = wp;
}
void del_wp(int n)
{
  if (n >= 0 && n < NR_WP)
    free_wp(wp_pool + n);
}

bool check_wp()
{
  bool flag = false;
  WP *wp = head;
  while (wp != NULL)
  {
    bool success;
    int result = expr(wp->args, &success);
    if (result != wp->value)
    {
      flag = true;
      printf("break point at %d : %s\n", wp->NO, wp->args);
    }
    wp->value = result;
    wp = wp->next;
  }
  return flag;
}

void print_wp()
{
  WP *wp = head;
  while (wp != NULL)
  {
    printf("%d : %s\n", wp->NO, wp->args);
    wp = wp->next;
  }
}
