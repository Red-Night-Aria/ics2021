#include <isa.h>
#include <cpu/cpu.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <utils.h>
#include "sdb.h"
#include <common.h>
#include <memory/paddr.h>

extern NEMUState nemu_state;

static int is_batch_mode = false;

void init_regex();
void init_wp_pool();
WP* new_wp();
void free_wp(uint32_t n);

/* We use the `readline' library to provide more flexibility to read from stdin. */
static char* rl_gets() {
  static char *line_read = NULL;

  if (line_read) {
    free(line_read);
    line_read = NULL;
  }

  line_read = readline("(nemu) ");

  if (line_read && *line_read) {
    add_history(line_read);
  }

  return line_read;
}

static int cmd_c(char *args) {
  cpu_exec(-1);
  return 0;
}


static int cmd_q(char *args) {
  nemu_state.state = NEMU_QUIT;
  return -1;
}

static int cmd_help(char *args);

// si N
static int cmd_si(char *args) {
  cpu_exec(1);
  return 0;
}

static int cmd_info(char* args) {
  if (strcmp(args, "r") == 0) {
    isa_reg_display();
  } else if (strcmp(args, "w") == 0) {
    print_wp_infos();
  } else {
    printf("Unknown subcommond '%s'\n", args);
  }
  return 0;
}

// x N EXPR
static int cmd_x(char* args) {
  // Parse Number Begin
  char *n_str = strtok(args, " ");
  if (n_str == NULL) {
    printf("Invalid commond format: %s\n", args);
    return 0;
  }

  char *end_ptr;
  uint32_t n = strtoul(n_str, &end_ptr, 10);
  if (end_ptr == n_str) {
    printf("Parse string to number fail: %s", n_str);
    return 0;
  }
  // Parse Number End

  char* expr_str = n_str + strlen(n_str) + 1;
  bool is_expr_valid = false;
  word_t begin_addr = expr(expr_str, &is_expr_valid);
  if (!is_expr_valid) {
    printf("Invalid expression: %s", expr_str);
    return 0;
  }
  for (int i = 0; i < n; ++i) {
    word_t addr_to_print = begin_addr + i * WORD_SIZE;
    word_t mem_value = paddr_read(addr_to_print, WORD_SIZE);
    printf(FMT_PADDR "\t" FMT_WORD "\n", addr_to_print, mem_value);
  }
  return 0;
}

static int cmd_p(char* args) {
  bool succ = false;
  word_t value = expr(args, &succ);
  if (succ) {
    printf("%d\n", value);
  } else {
    printf("Caculate fail.\n");
  }
  return 0;
}

static int cmd_w(char* args) {
  Assert(strlen(args) < MAX_EXPR_LEN, "expression too long"); 
  bool succ = false;
  word_t value = expr(args, &succ);
  if (!succ) {
    printf("Invaild Expression: %s\n", args);
    return 0;
  }

  WP* wp = new_wp();
  wp->expr_val = value;  
  strcpy(wp->expression, args);
  printf("Set watchpoint %d\n", wp->NO);
  return 0;
}

static int cmd_d(char* args) {
  char *end_ptr;
  uint32_t n = strtoul(args, &end_ptr, 0);
  if (end_ptr == args) {
    printf("Parse string to number fail: %s\n", args);
  } else {
    if (n >= NR_WP) {
      printf("Exceed range: %d\n", n);
    } else {
      free_wp(n);
    }
  }
  return 0;
}

uint32_t g_bp_addr = 0;  // breakpoint

static int cmd_b(char* args) {
  char *end_ptr;
  uint32_t n = strtoul(args, &end_ptr, 0);
  if (end_ptr == args) {
    printf("Parse string to number fail: %s\n", args);
  } else {
    g_bp_addr = n;
  }
  return 0;
}

static struct {
  const char *name;
  const char *description;
  int (*handler) (char *);
} cmd_table [] = {
  { "help", "Display informations about all supported commands", cmd_help },
  { "c", "Continue the execution of the program", cmd_c },
  { "q", "Exit NEMU", cmd_q },
  { "si", "Execute single instruction", cmd_si },
  { "info", "Print registers' value", cmd_info },
  { "x", "Print memory value", cmd_x },
  { "p", "Caculate expression", cmd_p},
  { "w", "Set watch point", cmd_w},
  { "d", "Delete watch point", cmd_d},
  { "b", "Set break point", cmd_b},
};

#define NR_CMD ARRLEN(cmd_table)

static int cmd_help(char *args) {
  /* extract the first argument */
  char *arg = strtok(NULL, " ");
  int i;

  if (arg == NULL) {
    /* no argument given */
    for (i = 0; i < NR_CMD; i ++) {
      printf("%s - %s\n", cmd_table[i].name, cmd_table[i].description);
    }
  }
  else {
    for (i = 0; i < NR_CMD; i ++) {
      if (strcmp(arg, cmd_table[i].name) == 0) {
        printf("%s - %s\n", cmd_table[i].name, cmd_table[i].description);
        return 0;
      }
    }
    printf("Unknown command '%s'\n", arg);
  }
  return 0;
}

void sdb_set_batch_mode() {
  is_batch_mode = true;
}

void sdb_mainloop() {
  if (is_batch_mode) {
    cmd_c(NULL);
    return;
  }

  for (char *str; (str = rl_gets()) != NULL; ) {
    char *str_end = str + strlen(str);

    /* extract the first token as the command */
    char *cmd = strtok(str, " ");
    if (cmd == NULL) { continue; }

    /* treat the remaining string as the arguments,
     * which may need further parsing
     */
    char *args = cmd + strlen(cmd) + 1;
    if (args >= str_end) {
      args = NULL;
    }

#ifdef CONFIG_DEVICE
    extern void sdl_clear_event_queue();
    sdl_clear_event_queue();
#endif

    int i;
    for (i = 0; i < NR_CMD; i ++) {
      if (strcmp(cmd, cmd_table[i].name) == 0) {
        if (cmd_table[i].handler(args) < 0) { return; }
        break;
      }
    }

    if (i == NR_CMD) { printf("Unknown command '%s'\n", cmd); }
  }
}

void init_sdb() {
  /* Compile the regular expressions. */
  init_regex();

  /* Initialize the watchpoint pool. */
  init_wp_pool();
}
