#include <am.h>
#include <amdev.h>
#include <nemu.h>
#include <string.h>

static int W;
static int H;
void get_rect(int *width, int *height);
int screen_width();
int screen_height();
void draw_sync();

#define min(x, y) (x < y ? x : y)

size_t __am_video_read(uintptr_t reg, void *buf, size_t size)
{
  switch (reg)
  {
  case _DEVREG_VIDEO_INFO:
  {
    _DEV_VIDEO_INFO_t *info = (_DEV_VIDEO_INFO_t *)buf;
    unsigned t = inl(SCREEN_ADDR);
    W = info->width = (t >> 16) & 0xffff;
    H = info->height = t & 0xffff;
    return sizeof(_DEV_VIDEO_INFO_t);
  }
  }
  return 0;
}

size_t __am_video_write(uintptr_t reg, void *buf, size_t size)
{
  switch (reg)
  {
  case _DEVREG_VIDEO_FBCTL:
  {
    _DEV_VIDEO_FBCTL_t *ctl = (_DEV_VIDEO_FBCTL_t *)buf;
    int x = ctl->x, y = ctl->y, w = ctl->w, h = ctl->h;
    uint32_t *pixels = ctl->pixels;
    int cp_bytes = sizeof(uint32_t) * min(w, W - x);
    uint32_t *dst = (uint32_t *)(uintptr_t)FB_ADDR;
    for (int j = 0; j < h && y + j < H; j++)
    {
      memcpy(dst + (y + j) * W + x, pixels, cp_bytes);
      pixels += w;
    }
    if (ctl->sync)
    {
      outl(SYNC_ADDR, 0);
    }
    return size;
  }
  }
  return 0;
}

void __am_vga_init()
{
  int i;
  get_rect(&W, &H);
  // W = screen_width();
  // H = screen_height();
  int size = W * H;
  uint32_t *fb = (uint32_t *)(uintptr_t)FB_ADDR;
  for (i = 0; i < size; i++)
    fb[i] = i;
  draw_sync();
}
