#ifndef __SDB_H__
#define __SDB_H__

#include <common.h>

word_t expr(char *e, bool *success);

#define MAX_EXPR_LEN 256

typedef struct watchpoint {
  int NO;
  struct watchpoint *next;

  /* TODO: Add more members if necessary */
  char expression[MAX_EXPR_LEN];
  uint32_t expr_val;
} WP;

bool check_if_watchpoint_change();

void print_wp_infos();

#define NR_WP 32

#endif
