#include <klib.h>
#include <klib-macros.h>
#include <stdint.h>

#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)

size_t strlen(const char *s) {
  size_t len = 0;
  while (s[len] != '\0') {
    ++len;
  }
  return len;
}

char *strcpy(char *dst, const char *src) {
  return strncpy(dst, src, strlen(src));
}

char *strncpy(char *dst, const char *src, size_t n) {
  // Should not overlapped since src is const
  if (!(dst > src + n || dst + n < src)) {
    panic("src and dst is overlapped");
  }

  size_t i;

  for (i = 0; i < n && src[i] != '\0'; i++)
    dst[i] = src[i];
  for (; i < n; i++)
    dst[i] = '\0';

  return dst;
}

char *strcat(char *dst, const char *src) {
  size_t dest_len = strlen(dst);
  size_t i;

  for (i = 0; src[i] != '\0'; i++)
    dst[dest_len + i] = src[i];

  dst[dest_len + i] = '\0';
  return dst;
}

int strcmp(const char *s1, const char *s2) {
  return strncmp(s1, s2, max(strlen(s1), strlen(s2)));
}

int strncmp(const char *s1, const char *s2, size_t n) {
  size_t cmp_idx = 0;
  while (cmp_idx < n) {
    if (s1[cmp_idx] < s2[cmp_idx]) {
      return -1;
    } else if (s1[cmp_idx] > s2[cmp_idx]) {
      return 1;
    }
    if (s1[cmp_idx] == '\0') {
      break;
    }
    ++cmp_idx;
  }
  return 0;
}

void *memset(void *s, int c, size_t n) {
  for (int i = 0; i < n; ++i) {
    ((int*)s)[i] = c;
  }
  return s;
}

void *memmove(void *dst, const void *src, size_t n) {
  panic("Not implemented");
}

void *memcpy(void *out, const void *in, size_t n) {
  if (!(out > in + n || out + n < in)) {
    panic("src and dst is overlapped");
  }

  for (int i = 0; i < n; ++i) {
    ((char*)out)[i] = ((char*)in)[i];
  }

  return out;
}

int memcmp(const void *s1, const void *s2, size_t n) {
  while (n > 0) {
    if (*(unsigned char*)s1 > *(unsigned char*)s2) {
      return 1;
    } else if (*(unsigned char*)s1 < *(unsigned char*)s2){
      return -1;
    }
    --n;
  }
  return 0;
}

#endif
