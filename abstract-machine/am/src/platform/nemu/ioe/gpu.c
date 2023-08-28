#include <am.h>
#include <nemu.h>

#define SYNC_ADDR (VGACTL_ADDR + 4)

void __am_gpu_config(AM_GPU_CONFIG_T *cfg) {
  *cfg = (AM_GPU_CONFIG_T) {
    .present = true, .has_accel = false,
    .width = 0, .height = 0,
    .vmemsz = 0
  };
  uint32_t vga_cfg = inl(VGACTL_ADDR);
  cfg->width = vga_cfg >> 16;
  cfg->height = vga_cfg & ((1 << 16) - 1);
}

void __am_gpu_init() {
  int i;
  AM_GPU_CONFIG_T cfg;
  __am_gpu_config(&cfg);
  int w = cfg.width;
  int h = cfg.height;
  uint32_t *fb = (uint32_t *)(uintptr_t)FB_ADDR;
  for (i = 0; i < w * h; i ++) fb[i] = i;
  outl(SYNC_ADDR, 1);
}

void __am_gpu_fbdraw(AM_GPU_FBDRAW_T *ctl) {
  AM_GPU_CONFIG_T cfg;
  __am_gpu_config(&cfg);
  int w = cfg.width;
  // int h = cfg.height;

  uint32_t *fb = (uint32_t *)(uintptr_t)FB_ADDR;
  for (int i = ctl->y; i < ctl->y + ctl->h; ++i) {
    uint32_t *start = fb + i * w;
    uint32_t *src_start = (uint32_t*)ctl->pixels + (i - ctl->y) * ctl->w;
    for (int j = ctl->x; j < ctl->x + ctl->w; ++j) {
      start[j] = src_start[j - ctl->x];
    }
  }

  if (ctl->sync) {
    outl(SYNC_ADDR, 1);
  }
}

void __am_gpu_status(AM_GPU_STATUS_T *status) {
  status->ready = true;
}
