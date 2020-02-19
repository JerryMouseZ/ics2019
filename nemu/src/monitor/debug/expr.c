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
  TK_HEX

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

    {" +", TK_NOTYPE},       // spaces
    {"[1-9][0-0]*", TK_DEC}, //dec
    {"0x[0-9]+", TK_HEX},    //hex
    {"\\+", '+'},            // plus
    {"\\-", '-'},            // sub
    {"\\*", '*'},            //mul
    {"\\/", '/'},            // div
    {"==", TK_EQ}            // equal
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
static int priority[32] = {0};
static Value values[32] = {0};
static int values_top = -1;
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
        case TK_HEX:
        case TK_DEC:
          tokens[nr_token].type = rules[i].token_type;
          strncpy(tokens[nr_token].str, substr_start, substr_len);
          priority[nr_token] = 0;
          nr_token++;
          break;
        case '+':
        case '-':
          tokens[nr_token].type = rules[i].token_type;
          strncpy(tokens[nr_token].str, substr_start, substr_len);
          priority[nr_token] = 1;
          nr_token++;
          break;
        case '*':
        case '/':
          tokens[nr_token].type = rules[i].token_type;
          strncpy(tokens[nr_token].str, substr_start, substr_len);
          priority[nr_token] = 2;
          nr_token++;
          break;
        default:
          break;
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

Value eval(Token token)
{
  Value value;
  switch (token.type)
  {
  case TK_DEC:
    value.value = atoi(token.str);
    break;
  case TK_HEX:
    value.value = atoi(token.str);
    break;
  case TK_EQ:
    value.value = '=';
    break;
  case '+':
    value.value = '+';
    break;
  case '-':
    value.value = '-';
    break;
  case '*':
    value.value = '*';
    break;
  case '/':
    value.value = '/';
    break;
  default:
    break;
  }
  return value;
}

uint32_t expr(char *e, bool *success)
{
  if (!make_token(e))
  {
    *success = false;
    return 0;
  }
  printf("token has been made\n");
  values_top = 0;
  values[values_top] = eval(tokens[0]);
  for (int i = 1; i < nr_token; i++)
  {
    if (priority[i] < priority[i - 1])
    {
      // 后面的优先级更大， 移进
      values[++values_top] = eval(tokens[i]);
      printf("%d\n", values[values_top].value);
    }
    else
    {
      //从栈中弹出三个元素，压入一个元素
      Value tmp;
      switch (values[values_top - 1].value)
      {
      case '+':
        tmp.value = values[values_top - 2].value + values[values_top].value;
        tmp.type = true;
        break;
      case '-':
        tmp.value = values[values_top - 2].value - values[values_top].value;
        tmp.type = true;
        break;
      case '*':
        tmp.value = values[values_top - 2].value * values[values_top].value;
        tmp.type = true;
        break;
      case '/':
        tmp.value = values[values_top - 2].value / values[values_top].value;
        tmp.type = true;
        break;
      }
      values_top -= 2;
      values[values_top] = tmp;
      printf("%d\n", values[values_top].value);
    }
  }
  /* TODO: Insert codes to evaluate the expression. */
  //TODO();
  return values[0].value;
}
