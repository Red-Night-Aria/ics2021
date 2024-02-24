#include <common.h>
#include <device.h>

#if defined(MULTIPROGRAM) && !defined(TIME_SHARING)
# define MULTIPROGRAM_YIELD() yield()
#else
# define MULTIPROGRAM_YIELD()
#endif

#define NAME(key) \
  [AM_KEY_##key] = #key,

static const char *keyname[256] __attribute__((used)) = {
  [AM_KEY_NONE] = "NONE",
  AM_KEYS(NAME)
};

size_t serial_write(const void *buf, size_t offset, size_t len) {
  const char* buf_ptr = buf;
  for (size_t i = 0; i < len; ++i) {
    putch(*buf_ptr);
    ++buf_ptr;
  }
  return len;
}

static char key_down[] = "kd ";
static char key_up[] = "ku ";
size_t events_read(void *buf, size_t offset, size_t len) {
  AM_INPUT_KEYBRD_T key_struct;
  ioe_read(AM_INPUT_KEYBRD, &key_struct);
  if (key_struct.keycode == 0) {
    return 0;
  }
  size_t part_1_len = 0;
  if (key_struct.keydown) {
    part_1_len = strlen(key_down);
    memcpy(buf, key_down, part_1_len);
  } else {
    part_1_len = strlen(key_up);
    memcpy(buf, key_up, part_1_len);
  }
  const char* keycode = keyname[key_struct.keycode];
  size_t real_size = part_1_len + strlen(keycode) + 2;
  assert(real_size <= len);
  memcpy(buf + 3, keycode, strlen(keycode));
  strcpy(buf + 3 + strlen(keycode), "\n");
  return real_size;
}

AM_GPU_CONFIG_T gpu_cfg;

size_t dispinfo_read(void *buf, size_t offset, size_t len) {
  return sprintf(buf, "WIDTH:%d\nHEIGHT:%d\n", gpu_cfg.width, gpu_cfg.height);
}

// update one row each call
size_t fb_write(const void *buf, size_t offset, size_t len) {
  AM_GPU_FBDRAW_T fb_draw;
  fb_draw.y = offset / gpu_cfg.width;
  fb_draw.x = offset % gpu_cfg.width;
  fb_draw.h = 1;
  fb_draw.w = len;
  fb_draw.pixels = (void*)buf;
  assert(fb_draw.w + fb_draw.x <= gpu_cfg.width);
  ioe_write(AM_GPU_FBDRAW, &fb_draw);
  return len;
}

int read_uptime(struct timeval* val) {
  if (val == 0) {
    return -1;
  }
  AM_TIMER_UPTIME_T buf;
  ioe_read(AM_TIMER_UPTIME, &buf);
  val->tv_sec = buf.us / 1000000;
  val->tv_usec = buf.us;
  return 0;
}

void init_device() {
  Log("Initializing devices...");
  ioe_init();
  ioe_read(AM_GPU_CONFIG, &gpu_cfg);
}
