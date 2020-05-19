#include "fs.h"

typedef size_t (*ReadFn)(void *buf, size_t offset, size_t len);
typedef size_t (*WriteFn)(const void *buf, size_t offset, size_t len);

size_t ramdisk_read(void *buf, size_t offset, size_t len);
size_t ramdisk_write(const void *buf, size_t offset, size_t len);
size_t serial_write(const void *buf, size_t offset, size_t len);
size_t events_read(void *buf, size_t offset, size_t len);
size_t dispinfo_read(void *buf, size_t offset, size_t len);
size_t fb_write(const void *buf, size_t offset, size_t len);
size_t fbsync_write(const void *buf, size_t offset, size_t len);

extern int screen_width_, screen_height_;

typedef struct
{
  char *name;
  size_t size;
  size_t disk_offset;
  size_t open_offset;
  ReadFn read;
  WriteFn write;
} Finfo;

enum
{
  FD_STDIN,
  FD_STDOUT,
  FD_STDERR,
  FD_FB
};

size_t invalid_read(void *buf, size_t offset, size_t len)
{
  panic("should not reach here");
  return 0;
}

size_t invalid_write(const void *buf, size_t offset, size_t len)
{
  panic("should not reach here");
  return 0;
}

/* This is the information about all files in disk. */
static Finfo file_table[] __attribute__((used)) = {
    {"stdin", 0, 0, 0, invalid_read, invalid_write},
    {"stdout", 0, 0, 0, invalid_read, serial_write},
    {"stderr", 0, 0, 0, invalid_read, serial_write},
    {"/dev/events", 0, 0, 0, events_read, serial_write},
    {"/dev/fb", 0, 0, 0, NULL, fb_write},
    {"/dev/fbsync", 0, 0, 0, NULL, fbsync_write},
    {"/proc/dispinfo", 0, 0, 0, dispinfo_read, NULL},
    {"/dev/tty", 0, 0, 0, NULL, serial_write},
#include "files.h"
};

#define NR_FILES (sizeof(file_table) / sizeof(file_table[0]))

void init_fs()
{
  // TODO: initialize the size of /dev/fb
  file_table[4].size = screen_width_ * screen_height_ * sizeof(uint32_t);
  Log("fb size : %d", file_table[4].size);
  for (int i = 7; i < NR_FILES; i++)
  {
    file_table[i].open_offset = file_table[i].disk_offset;
  }
}

int fs_open(const char *pathname, int flags, int mode)
{
  for (int i = 0; i < NR_FILES; i++)
  {
    if (strcmp(pathname, file_table[i].name) == 0)
    {
      // need to reset the open offset, if it is reopened
      file_table[i].open_offset = file_table[i].disk_offset;
      return i;
    }
  }
  return -1;
}

inline static void check_len(size_t *len, int fd)
{
  if (file_table[fd].open_offset + *len > file_table[fd].disk_offset + file_table[fd].size)
  {
    *len = file_table[fd].disk_offset + file_table[fd].size - file_table[fd].open_offset;
  }
}

size_t fs_read(int fd, void *buf, size_t len)
{
  if (file_table[fd].read == NULL)
  { // normal file
    check_len(&len, fd);
    len = ramdisk_read(buf, file_table[fd].open_offset, len);
  }
  else
  {
    len = file_table[fd].read(buf, file_table[fd].open_offset, len);
  }
  file_table[fd].open_offset += len;
  return len;
}

size_t fs_write(int fd, const void *buf, size_t len)
{
  if (file_table[fd].write == NULL)
  { // normal file
    check_len(&len, fd);
    len = ramdisk_write(buf, file_table[fd].open_offset, len);
  }
  else
  {
    len = file_table[fd].write(buf, file_table[fd].open_offset, len);
  }
  file_table[fd].open_offset += len;
  return len;
}

size_t fs_lseek(int fd, size_t offset, int whence)
{
  switch (whence)
  {
  case SEEK_SET:
    file_table[fd].open_offset = file_table[fd].disk_offset + offset;
    break;
  case SEEK_CUR:
    file_table[fd].open_offset += offset;
    break;
  case SEEK_END:
    file_table[fd].open_offset = file_table[fd].size + offset;
    break;
  }
  return file_table[fd].open_offset;
}

int fs_close(int fd)
{
  return 0;
}