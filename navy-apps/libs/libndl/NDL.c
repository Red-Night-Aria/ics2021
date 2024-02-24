#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>

static int evtdev = -1;
static int fbdev = -1;
static int screen_w = 0, screen_h = 0;
static int canvas_w = 0, canvas_h = 0;

int gettimeofday(struct timeval *tv, struct timezone *tz);
uint32_t NDL_GetTicks() {
  struct timeval start; 
  gettimeofday(&start, NULL);
  return start.tv_usec;
}

static char keyboard_fname[] = "/dev/events";
static char dispinfo_fname[] = "/proc/dispinfo";
static char fb_fname[] = "/dev/fb";

int open(const char *pathname, int flags);

int NDL_PollEvent(char *buf, int len) {
  if (evtdev == -1) {
    evtdev = open(keyboard_fname, 0);
  }
  int res = 0;
  while (res == 0) {
    res = read(evtdev, buf, len);
  }
  return res;
}

#define CHECK(cond, format, ...) { if (!(cond)) { printf("CHECK FAIL: File %s, Line %d\n", __FILE__, __LINE__); printf(format, ##__VA_ARGS__); exit(-1);} }

#define panic(format, ...) CHECK(0, format, ## __VA_ARGS__)

void NDL_OpenCanvas(int *w, int *h) {
  if (getenv("NWM_APP")) {
    int fbctl = 4;
    fbdev = 5;
    screen_w = *w; screen_h = *h;
    char buf[64];
    int len = sprintf(buf, "%d %d", screen_w, screen_h);
    // let NWM resize the window and create the frame buffer
    write(fbctl, buf, len);
    while (1) {
      // 3 = evtdev
      int nread = read(3, buf, sizeof(buf) - 1);
      if (nread <= 0) continue;
      buf[nread] = '\0';
      if (strcmp(buf, "mmap ok") == 0) break;
    }
    close(fbctl);
  } else {
    if (screen_w == 0) {
      int fd = open(dispinfo_fname, 0);
      char buffer[1024];
      int cnt = read(fd, buffer, 1024);
      // printf("dispinfo:\n%s", buffer);
      char* lines[64] = {};
      char* token = strtok(buffer, "\n");
      int idx = 0;
      while (token != NULL) {
        char* line = malloc(1024);
        strcpy(line, token);
        lines[idx] = line;
        ++idx;
        token = strtok(NULL, "\n");
      }
      for (int i = 0; i < idx; ++i) {
        strcpy(buffer, lines[i]);
        char* key = strtok(buffer, ":");
        CHECK(key != NULL, "content: %s", lines[i]);
        char* val = strtok(NULL, ":");
        CHECK(val != NULL, "content: %s", lines[i]);
        if (strcmp(key, "WIDTH") == 0) {
          screen_w = atoi(val);
        } else if (strcmp(key, "HEIGHT") == 0) {
          screen_h = atoi(val);
        } else {
          panic("Unknown key: %s", key);
        }
      }
      if (*w == 0) {
        *w = screen_w;
        *h = screen_h;
      }
      CHECK(*w <= screen_w && *h <= screen_h, "Too large: %d * %d", *w, *h);
      canvas_h = *h;
      canvas_w = *w;
    }
    
    printf("Display Info: %d * %d\n", screen_w, screen_h);
  }
}

void NDL_DrawRect(uint32_t *pixels, int x, int y, int w, int h) {
  CHECK(screen_w != 0, "NDL_OpenCanvas hasn't been call");
  if (fbdev == -1) {
    fbdev = open(fb_fname, 0);
  }
  CHECK(canvas_w >= x + w, "Width invalid: %d %d %d", canvas_w, x, w)
  CHECK(canvas_h >= y + h, "Height invalid: %d %d %d", canvas_h, y, h)

  // LeftTop
  uint32_t offset = screen_w * y + x;
  lseek(fbdev, offset, SEEK_SET);
  for (int i = 0; i < h; ++i) {
    write(fbdev, pixels + i * w, w);
    offset += screen_w;
    lseek(fbdev, offset, SEEK_SET);
  }
}

void NDL_OpenAudio(int freq, int channels, int samples) {
}

void NDL_CloseAudio() {
}

int NDL_PlayAudio(void *buf, int len) {
  return 0;
}

int NDL_QueryAudio() {
  return 0;
}

int NDL_Init(uint32_t flags) {
  if (getenv("NWM_APP")) {
    evtdev = 3;
  }
  return 0;
}

void NDL_Quit() {
}
