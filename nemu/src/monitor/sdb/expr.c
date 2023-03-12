#include <isa.h>

/* We use the POSIX regex functions to process regular expressions.
 * Type 'man regex' for more information about POSIX regex functions.
 */
#include <regex.h>

enum {
  TK_NOTYPE = 256, TK_EQ,

  /* TODO: Add more token types */
  TK_NUM,
  TK_REG
};

static struct rule {
  const char *regex;
  int token_type;
} rules[] = {

  /* TODO: Add more rules.
   * Pay attention to the precedence level of different rules.
   */

  {" +", TK_NOTYPE},    // spaces
  {"\\+", '+'},         // plus
  {"==", TK_EQ},        // equal
  {"\\-", '-'},         // minus
  {"\\*", '*'},         // multi
  {"\\/", '/'},         // div
  {"\\(", '('},
  {"\\)", ')'},
  {"[0-9]+", TK_NUM},
  {"\\$0", TK_REG},
  {"\\$[A-Za-z0-9]{1,2}", TK_REG},
};

static int op_pri(int k) {
  switch (k)
  {
  case '+':
    return 10;
  case '-':
    return 10;
  case '*':
    return 20;
  case '/':
    return 20;
  default:
    return -1;
  }
}

#define NR_REGEX ARRLEN(rules)

static regex_t re[NR_REGEX] = {};

/* Rules are used for many times.
 * Therefore we compile them only once before any usage.
 */
void init_regex() {
  int i;
  char error_msg[128];
  int ret;

  for (i = 0; i < NR_REGEX; i ++) {
    ret = regcomp(&re[i], rules[i].regex, REG_EXTENDED);
    if (ret != 0) {
      regerror(ret, &re[i], error_msg, 128);
      panic("regex compilation failed: %s\n%s", error_msg, rules[i].regex);
    }
  }
}

#define BUFFER_LEN 32

typedef struct token {
  int type;
  char str[BUFFER_LEN];
} Token;

static Token tokens[BUFFER_LEN] __attribute__((used)) = {};
static int nr_token __attribute__((used))  = 0;

static bool make_token(char *e) {
  int position = 0;
  int i;
  regmatch_t pmatch;

  nr_token = 0;

  while (e[position] != '\0') {
    /* Try all rules one by one. */
    for (i = 0; i < NR_REGEX; i ++) {
      if (regexec(&re[i], e + position, 1, &pmatch, 0) == 0 && pmatch.rm_so == 0) {
        char *substr_start = e + position;
        int substr_len = pmatch.rm_eo;

        #ifdef CONFIG_DEBUG_EXPR
        Log("match rules[%d] = \"%s\" at position %d with len %d: %.*s",
            i, rules[i].regex, position, substr_len, substr_len, substr_start);
        #endif
        position += substr_len;

        /* TODO: Now a new token is recognized with rules[i]. Add codes
         * to record the token in the array `tokens'. For certain types
         * of tokens, some extra actions should be performed.
         */

        switch (rules[i].token_type) {
          case TK_NOTYPE:
            break;
          default:
            if (substr_len >= BUFFER_LEN || nr_token >= BUFFER_LEN) {
              printf("Token exceed buffer.");
              return false;
            }
            tokens[nr_token].type = rules[i].token_type;
            strncpy(tokens[nr_token].str, substr_start, substr_len);
            ++nr_token;
        }

        if (rules[i].token_type == TK_REG) {
          char* reg_name = tokens[nr_token - 1].str;
          bool reg_name_invalid = true;
          isa_reg_str2val(reg_name, &reg_name_invalid);
          if (!reg_name_invalid) {
            printf("Invalid reg name: %s\n", reg_name);
            return false;
          }
        }
        break;
      }
    }

    if (i == NR_REGEX) {
      printf("no match at position %d\n%s\n%*.s^\n", position, e, position, "");
      return false;
    }
  }

  return true;
}

bool check_parentheses(int l, int r, bool* is_valid) {
  if (tokens[l].type != '(' || tokens[r].type != ')') {
    return false;
  }

  bool is_pair = true;
  int l_pa_num = 0;
  for (int i = l; i < r; ++i) {
    if (tokens[i].type == '(') {
      ++l_pa_num;
    } else if (tokens[i].type == ')') {
      --l_pa_num;
      if (l_pa_num == 0) {
        is_pair = false;
      }
      if (l_pa_num < 0) {
        *is_valid = false;
        return false;
      }
    }
  }

  return is_pair;
}

int find_main_op(int l, int r) {
  int main_op_idx = -1, main_op_pri = INT32_MAX;
  int l_pa_num = 0;
  for (int i = l; i <= r; ++i) {
    if (tokens[i].type == '(') {
      ++l_pa_num;
      continue;
    } else if (tokens[i].type == ')') {
      --l_pa_num;
      if (l_pa_num < 0) {
        return -1;
      }
      continue;
    }

    int cur_op_pri = op_pri(tokens[i].type);
    if (cur_op_pri == -1) {
      continue;
    }
    if (l_pa_num > 0) {
      continue;
    }
    if (cur_op_pri <= main_op_pri) {
      main_op_idx = i;
      main_op_pri = cur_op_pri;
    }
  }
  // Redundant check?
  if (l_pa_num != 0) {
    return -1;
  }
  return main_op_idx;
}

#define FALSE_BRANCH(msg) \
printf("Bad expression: " msg " l: %d, r:%d\n", l, r); \
*succ = false; \
return 0;

int eval(int l, int r, bool* succ) {
  *succ = true;
  if (l > r) {
    FALSE_BRANCH("l > r")
  }

  bool reg_succ = false;
  word_t reg_val = 0;
  if (l == r) {
    switch (tokens[l].type) {
    case TK_NUM:
      return strtol(tokens[l].str, NULL, 0);
    case TK_REG:
      reg_val = isa_reg_str2val(tokens[l].str, &reg_succ);
      Assert(reg_succ, "Invalid register token: %s", tokens[l].str);
      return reg_val;
    default:
      FALSE_BRANCH("Unexpect token type")
    }
  }

  bool is_valid = true;
  if (check_parentheses(l, r, &is_valid)) {
    return eval(l + 1, r - 1, succ);
  }
  if (!is_valid) {
    FALSE_BRANCH("check_parentheses fail")
  }

  int k = find_main_op(l, r);
  if (k == -1) {
    FALSE_BRANCH("find_main_op fail")
  }

  bool l_succ = true, r_succ = true;
  int l_val = eval(l, k - 1, &l_succ), r_val = eval(k + 1, r, &r_succ);
  if (!l_succ || !r_succ) {
    FALSE_BRANCH("Sub eval fail")
  }

  switch (tokens[k].type) {
  case '+':
    return l_val + r_val;
  case '-':
    return l_val - r_val;
  case '*':
    return l_val * r_val;
  case '/':
    return l_val / r_val;
  default:
    panic("Shouldn't be here. k: %d", k);
  }
}

word_t expr(char *e, bool *success) {
  *success = true;

  if (!make_token(e)) {
    *success = false;
    return 0;
  }

  return eval(0, nr_token - 1, success);
}
