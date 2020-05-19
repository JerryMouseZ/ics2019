#include "common.h"
#include <amdev.h>

int screen_width_, screen_height_;

size_t serial_write(const void *buf, size_t offset, size_t len)
{
  int ret = 0;
  char *str = (char *)buf;
  for (ret = 0; ret < len; ret++)
  {
    // don't care the fail case
    _putc(str[ret]);
  }
  return ret;
}

#define NAME(key) \
  [_KEY_##key] = #key,

static const char *keyname[256] __attribute__((used)) = {
    [_KEY_NONE] = "NONE",
    _KEYS(NAME)};

size_t events_read(void *buf, size_t offset, size_t len)
{
  int ret = 0;
  char rec_buf[40];
  int key = read_key();
  int down = 0;
  if (key & 0x8000)
  {
    key ^= 0x8000;
    down = 1;
  }
  if (key != _KEY_NONE)
  {
    sprintf(rec_buf, "%s %s\n", down ? "kd" : "ku", keyname[key]);
    len -= strlen(rec_buf);
    if (len < 0)
      return -1;
    ret = strlen(rec_buf);
    strcpy(buf, rec_buf);
    return ret;
  }

  uint32_t microsec = uptime();
  // Log("micro second %d", microsec);
  if (len > 0)
  {
    memset(rec_buf, 0, sizeof(rec_buf));
    rec_buf[0] = 't';
    sprintf(rec_buf, "t %d\n", microsec);
    len -= strlen(rec_buf);
    if (len < 0)
      return strlen(buf);
    sprintf(buf + ret, "%s", rec_buf);
    ret += strlen(rec_buf);
  }
  return ret == 0 ? -1 : ret;
}

static char dispinfo[128] __attribute__((used)) = {};

size_t dispinfo_read(void *buf, size_t offset, size_t len)
{
  if (offset >= 128)
    return -1;
  strncpy(buf, dispinfo + offset, len);
  return len;
}

size_t fb_write(const void *buf, size_t offset, size_t len)
{
  offset /= sizeof(uint32_t);
  int x = offset % screen_width_;

  int y = offset / screen_width_;
  int w = len / 4;
  int h = 1;
  draw_rect((uint32_t *)buf, x, y, w, h);
  return len;
}

size_t fbsync_write(const void *buf, size_t offset, size_t len)
{
  draw_sync();
}

void init_device()
{
  Log("Initializing devices...");
  _ioe_init();
  // TODO: print the string to array `dispinfo` with the format
  // described in the Navy-apps convention
  screen_width_ = screen_width();
  screen_height_ = screen_height();
  sprintf(dispinfo, "WIDTH:%d\n", screen_width_);
  sprintf(dispinfo + strlen(dispinfo), "HEIGHT:%d\n", screen_height_);
  Log("%s", dispinfo);
}
