#include <am.h>
#include <klib.h>
#include <klib-macros.h>
#include <stdarg.h>

#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)

int printf(const char *fmt, ...) {
  va_list ap;
  va_start(ap, fmt);
  char print_str[4096];
  memset(print_str, '\0', 4096 / sizeof(int));
  int ret_code = vsprintf(print_str, fmt, ap);
  va_end(ap);
  if (ret_code > 0) {
    putstr(print_str);
  }
  return ret_code;
}

int vsprintf(char *out, const char *fmt, va_list ap) {
  int d;
  char* s;
  char* str_beg = out;
  char c;
  for (int i = 0; i < strlen(fmt);) {
    if (fmt[i] != '%') {
      *out = fmt[i];
      ++i; ++out;
    } else {
      switch (fmt[i+1]) {
      case 'd':
        d = va_arg(ap, int);
        int2str(d, out);
        out = out + strlen(out);
        break;
      case 's':
        s = va_arg(ap, char*);
        strcpy(out, s);
        out = out + strlen(out);
        break;
      case 'c':
        c = va_arg(ap, int);
        *out = c;
        out = out + 1;
        break;
      default:
        putch(fmt[i+1]);
        putch('\n');
        putstr(fmt);
        panic("Not implemented format type: ");
        return -1;
      }
      i += 2;
    }
  }
  *out = '\0';
  return strlen(str_beg);
}

int sprintf(char *out, const char *fmt, ...) {
  va_list ap;
  va_start(ap, fmt);
  int cnt = vsprintf(out, fmt, ap);
  va_end(ap);
  return cnt;
}

int snprintf(char *out, size_t n, const char *fmt, ...) {
  panic("Not implemented");
}

int vsnprintf(char *out, size_t n, const char *fmt, va_list ap) {
  panic("Not implemented");
}

#endif
