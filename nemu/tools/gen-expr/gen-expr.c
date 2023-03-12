#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <assert.h>
#include <string.h>

// this should be enough
static char buf[65536] = {};
static uint32_t buf_idx = 0;
static int exceed = 0;
static char code_buf[65536 + 128] = {}; // a little larger than `buf`
static char *code_format =
"#include <stdio.h>\n"
"int main() { "
"  unsigned result = %s; "
"  printf(\"%%u\", result); "
"  return 0; "
"}";

uint32_t choose(uint32_t n) {
  return rand() % n;
}

void gen_num() {
  uint32_t rand_num = rand() % 65535;
  char tmp_buf[20];
  sprintf(tmp_buf, "%d", rand_num);
  uint32_t tmp_buf_len = strlen(tmp_buf);
  if (buf_idx + tmp_buf_len >= 65535) {
    exceed = 1;
  } else {
    strcpy(buf + buf_idx, tmp_buf);
    buf_idx += tmp_buf_len;
  }
}

void gen(char c) {
  if (buf_idx + 1 >= 65535) {
    exceed = 1;
  } else {
    buf[buf_idx++] = c;
  }
}

void gen_rand_op() {
   if (buf_idx + 1 >= 65535) {
    exceed = 1;
  } else {
    char c;
    switch (choose(4)) {
    case 0:
      c = '+';
      break;
    case 1:
      c = '-';
      break;
    case 2:
      c = '*';
      break;
    case 3:
      c = '/';
      break;
    default:
      break;
    }
    buf[buf_idx++] = c;
  } 
}

static void gen_rand_expr() {
  switch (choose(3)) {
    case 0: gen_num(); break;
    case 1: gen('('); gen_rand_expr(); gen(')'); break;
    default: gen_rand_expr(); gen_rand_op(); gen_rand_expr(); break;
  }
}

int main(int argc, char *argv[]) {
  int seed = time(0);
  srand(seed);
  int loop = 1;
  if (argc > 1) {
    sscanf(argv[1], "%d", &loop);
  }
  int i;
  for (i = 0; i < loop; i ++) {
    buf_idx = 0, exceed = 0;
    gen_rand_expr();
    if (exceed) {
      continue;
    } else {
      buf[buf_idx] = '\0';
    }
    sprintf(code_buf, code_format, buf);

    FILE *fp = fopen("/tmp/.code.c", "w");
    assert(fp != NULL);
    fputs(code_buf, fp);
    fclose(fp);

    int ret = system("gcc /tmp/.code.c -o /tmp/.expr");
    if (ret != 0) continue;

    fp = popen("/tmp/.expr", "r");
    assert(fp != NULL);

    int result = 0;
    ret = fscanf(fp, "%d", &result);
    pclose(fp);

    printf("%u %s\n", result, buf);
  }
  return 0;
}
