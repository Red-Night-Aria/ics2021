#include <am.h>
#include <klib.h>
#include <klib-macros.h>
#include <stdarg.h>

#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)

int printf(const char *fmt, ...) {
  panic("Not implemented");
}

int vsprintf(char *out, const char *fmt, va_list ap) {
  panic("Not implemented");
}

int sprintf(char *out, const char *fmt, ...) {
  va_list ap;
  va_start(ap, fmt);
  int d;
  char* s;
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
      default:
        panic("Not implemented format type");
        break;
      }
      i += 2;
    }
  }
  *out = '\0';
  return 0;
}

int snprintf(char *out, size_t n, const char *fmt, ...) {
  panic("Not implemented");
}

int vsnprintf(char *out, size_t n, const char *fmt, va_list ap) {
  panic("Not implemented");
}

#endif
