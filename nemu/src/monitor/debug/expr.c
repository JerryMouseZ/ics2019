#include "nemu.h"
#include "stdlib.h"
/* We use the POSIX regex functions to process regular expressions.
 * Type 'man regex' for more information about POSIX regex functions.
 */
#include <sys/types.h>
#include <regex.h>

enum
{
  TK_NOTYPE = 256,
  TK_EQ,
  TK_DEC,
  TK_HEX,
  TK_REG
  /* TODO: Add more token types */

};

static struct rule
{
  char *regex;
  int token_type;
} rules[] = {

    /* TODO: Add more rules.
   * Pay attention to the precedence level of different rules.
   */

    {" +", TK_NOTYPE},                   // spaces
    {"[1-9][0-0]*", TK_DEC},             //dec
    {"0x[0-9]+", TK_HEX},                //hex
    {"\\+", '+'},                        // plus
    {"\\-", '-'},                        // sub
    {"\\*", '*'},                        //mul
    {"\\/", '/'},                        // div
    {"==", TK_EQ},                       // equal
    {"\\$[eE][a-zA-Z][a-zA-Z]", TK_REG}, //REG
    {"\\(", '('},                        // LEF
    {"\\)", ')'}                         //RIG
};

#define NR_REGEX (sizeof(rules) / sizeof(rules[0]))

static regex_t re[NR_REGEX] = {};

/* Rules are used for many times.
 * Therefore we compile them only once before any usage.
 */
void init_regex()
{
  int i;
  char error_msg[128];
  int ret;

  for (i = 0; i < NR_REGEX; i++)
  {
    ret = regcomp(&re[i], rules[i].regex, REG_EXTENDED);
    if (ret != 0)
    {
      regerror(ret, &re[i], error_msg, 128);
      panic("regex compilation failed: %s\n%s", error_msg, rules[i].regex);
    }
  }
}

typedef struct token
{
  int type;
  char str[32];
} Token;

typedef struct Value
{
  bool type;
  int value;
} Value;

static Token tokens[32] __attribute__((used)) = {};
static int priority = 0;
static int values[32] = {0};
static int ops[32] = {0};
static int values_top = -1;
static int ops_top = -1;
static int nr_token __attribute__((used)) = 0;

static bool make_token(char *e)
{
  int position = 0;
  int i;
  regmatch_t pmatch;

  nr_token = 0;

  while (e[position] != '\0')
  {
    /* Try all rules one by one. */
    for (i = 0; i < NR_REGEX; i++)
    {
      if (regexec(&re[i], e + position, 1, &pmatch, 0) == 0 && pmatch.rm_so == 0)
      {
        char *substr_start = e + position;
        int substr_len = pmatch.rm_eo;

        position += substr_len;

        /* TODO: Now a new token is recognized with rules[i]. Add codes
         * to record the token in the array `tokens'. For certain types
         * of tokens, some extra actions should be performed.
         */

        switch (rules[i].token_type)
        {
        case TK_NOTYPE:
          break;
        default:
          tokens[nr_token].type = rules[i].token_type;
          strncpy(tokens[nr_token].str, substr_start, substr_len);
          nr_token++;
          //TODO();
        }
        break;
      }
    }

    if (i == NR_REGEX)
    {
      printf("no match at position %d\n%s\n%*.s^\n", position, e, position, "");
      return false;
    }
  }

  return true;
}

int eval(Token token)
{
  int num = 0;
  switch (token.type)
  {
  case TK_REG:
    for (int j = R_EAX; j <= R_EDI; j++)
    {
      if (!strcmp(token.str + 1, reg_name(j, 4)))
      {
        return reg_l(j);
      }
    }
    if (strcmp(token.str + 1, "eip"))
      return cpu.pc;
  case TK_DEC:
    return atoi(token.str);
  case TK_HEX:
    for (char *begin = token.str + 2; begin < token.str + strlen(token.str); begin++)
    {
      num *= 16;
      if (*begin >= 'a' && *begin <= 'f')
        num += *begin - 'a' + 10;
      else if (*begin >= '0' && *begin <= '9')
        num += *begin - '0';
    }
    return num;
  case TK_EQ:
    return '=';
  case '+':
    return '+';
  case '-':
    return '-';
  case '*':
    return '*';
  case '/':
    return '/';
  case '(':
    return '(';
  case ')':
    return ')';
  default:
    break;
  }
  return 0;
}

int get_post_priority(int op)
{
  switch (op)
  {
  case '+':
  case '-':
    return 1;
  case '*':
  case '/':
    return 2;
  case '(':
    return 3;
  case ')':
    return 3;
  }
  return 0;
}
int get_pre_priority(int op)
{
  switch (op)
  {
  case '+':
  case '-':
    return 1;
  case '*':
  case '/':
    return 2;
  case '(':
    return 0;
  case ')':
    return 3;
    break;
  }
  return 0;
}

uint32_t expr(char *e, bool *success)
{
  if (!make_token(e))
  {
    *success = false;
    return 0;
  }
  values_top = -1;
  ops_top = -1;
  priority = 0;
  int tmp_pri = 0;
  for (int i = 0; i < nr_token; i++)
  {
    // 括号应该加到符号栈里，这样就可以和前面的运算符隔开了
    switch (tokens[i].type)
    {
    case TK_DEC:
    case TK_HEX:
    case TK_REG:
      values[++values_top] = eval(tokens[i]);
      break;
    default:

      if (get_post_priority(eval(tokens[i])) > get_pre_priority(ops[ops_top]))
      {
        printf("shift\n");
        ops[++ops_top] = eval(tokens[i]);
        priority = tmp_pri;
        if (priority == 3)
          priority = 0;
      }
      else
      {
        printf("reduce\n");
        int tmp = 0;
        switch (ops[ops_top])
        {
        case '+':
          tmp = values[values_top] + values[values_top - 1];
          break;
        case '-':
          tmp = values[values_top] - values[values_top - 1];
          break;
        case '*':
          tmp = values[values_top] * values[values_top - 1];
          break;
        case '/':
          tmp = values[values_top] / values[values_top - 1];
          break;
        case ')':
          tmp = values[values_top];
          values_top++;
          ops_top--;
          break;
        }
        values[--values_top] = tmp;
        ops[ops_top] = eval(tokens[i]);
        priority = tmp_pri;
      }
    }
  }
  *success = true;
  for (int i = 0; i <= ops_top; i++)
  {
    printf("%c\n", ops[i]);
  }
  for (int i = 0; i <= values_top; i++)
    printf("%d\n", values[i]);
  while (values_top > 0)
  {
    int tmp = 0;
    switch (ops[ops_top])
    {
    case '+':
      tmp = values[values_top] + values[values_top - 1];
      break;
    case '-':
      tmp = values[values_top] - values[values_top - 1];
      break;
    case '*':
      tmp = values[values_top] * values[values_top - 1];
      break;
    case '/':
      tmp = values[values_top] / values[values_top - 1];
      break;
    case ')':
      tmp = values[values_top];
      values_top++;
      ops_top--;
    }
    ops_top--;
    values[--values_top] = tmp;
  }
  /* TODO: Insert codes to evaluate the expression. */
  //TODO();
  return values[0];
}
