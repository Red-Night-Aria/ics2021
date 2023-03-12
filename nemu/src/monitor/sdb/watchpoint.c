#include "sdb.h"

static WP wp_pool[NR_WP] = {};
static WP *head = NULL, *free_ = NULL;

void init_wp(WP *wp) {
  wp->expr_val = 0;
}

WP* new_wp() {
  assert(free_ != NULL);
  WP *free_wp = free_;

  // adjust free list
  free_ = free_->next;

  // adjust used list
  free_wp->next = head;
  head = free_wp;

  init_wp(free_wp);
  return free_wp;
}

void free_wp(uint32_t n) {
  // remove wp from used list
  WP* iter = head;
  WP* prev = NULL;
  while (iter != NULL) {
    if (iter->NO == n) {
      break;
    }
    prev = iter;
    iter = iter->next;
  }

  if (iter == NULL) {
    printf("WatchPoint %d not exist\n", n);
    return;
  }

  if (prev != NULL) {
    prev->next = iter->next;
  } else {
    head = iter->next;
  }

  iter->next = free_;
  free_ = iter;
}

void init_wp_pool() {
  int i;
  for (i = 0; i < NR_WP; i ++) {
    wp_pool[i].NO = i;
    wp_pool[i].next = (i == NR_WP - 1 ? NULL : &wp_pool[i + 1]);
  }

  head = NULL;
  free_ = wp_pool;
}

/* TODO: Implement the functionality of watchpoint */

void print_wp_infos() {
  WP* iter = head;
  while (iter != NULL) {
    printf("WatchPoint %d, Expr: %s, Val: %u\n", iter->NO, iter->expression, iter->expr_val);
    iter = iter->next;
  }
}

bool check_if_watchpoint_change() {
  WP* iter = head;
  bool changed = false;
  while (iter != NULL) {
    bool expr_succ = true;
    word_t new_val = expr(iter->expression, &expr_succ);
    if (expr_succ && new_val != iter->expr_val) {
      printf("WatchPoint %d changed. Prev value: %u, New value: %u\n", iter->NO, iter->expr_val, new_val);
      iter->expr_val = new_val;
      changed = true;
    }
    iter = iter->next;
  }
  return changed;
}
