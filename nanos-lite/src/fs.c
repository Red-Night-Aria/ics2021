#include <fs.h>
#include "device.h"

typedef size_t (*ReadFn) (void *buf, size_t offset, size_t len);
typedef size_t (*WriteFn) (const void *buf, size_t offset, size_t len);
size_t serial_write(const void *buf, size_t offset, size_t len);

typedef struct {
  char *name;
  size_t size;
  size_t disk_offset;
  ReadFn read;
  WriteFn write;
  size_t open_offset;  // temp
} Finfo;

enum {FD_STDIN, FD_STDOUT, FD_STDERR, FD_FB, FD_EVENTS, FD_DISPINFO};

size_t invalid_read(void *buf, size_t offset, size_t len) {
  panic("should not reach here");
  return 0;
}

size_t invalid_write(const void *buf, size_t offset, size_t len) {
  panic("should not reach here");
  return 0;
}

/* This is the information about all files in disk. */
static Finfo file_table[] __attribute__((used)) = {
  [FD_STDIN]  = {"stdin", 0, 0, invalid_read, invalid_write},
  [FD_STDOUT] = {"stdout", 0, 0, invalid_read, serial_write},
  [FD_STDERR] = {"stderr", 0, 0, invalid_read, serial_write},
  [FD_FB] = {"/dev/fb", 0, 0, invalid_read, invalid_write},
  [FD_EVENTS] = {"/dev/events", 0, 0, events_read, invalid_write},
  [FD_DISPINFO] = {"/proc/dispinfo", 0, 0, dispinfo_read, invalid_write},
#include "files.h"
};

void init_fs() {
  AM_GPU_CONFIG_T gpu_struct;
  ioe_read(AM_GPU_CONFIG, &gpu_struct);
  if (gpu_struct.present) {
    file_table[FD_FB].size = gpu_struct.height * gpu_struct.width; 
    file_table[FD_FB].write = fb_write; 
  }
}

int fs_open(const char *pathname, int flags, int mode) {
  for (int i = 0; i < ARRAY_LEN(file_table); ++i) {
    if (strcmp(pathname, file_table[i].name) == 0) {
      return i;
    }
  }
  panic("File not found: %s", pathname);
}

size_t ramdisk_read(void *buf, size_t offset, size_t len);
size_t ramdisk_write(const void *buf, size_t offset, size_t len);

bool is_block_dev(Finfo* finfo) {
  return finfo->size != 0;
}

Finfo* find_finfo(int fd) {
  if (fd < 0 || fd >= ARRAY_LEN(file_table)) {
    panic("Invalid fd: %d", fd);
  }
  return file_table + fd;
}

size_t fs_read(int fd, void *buf, size_t len) {
  Finfo* f_info = find_finfo(fd);
  if (is_block_dev(f_info)) {
    size_t cur_offset = f_info->disk_offset + f_info->open_offset;
    size_t remain_bytes = f_info->size - f_info->open_offset;
    size_t bytes_to_read = len > remain_bytes ? remain_bytes : len;
    size_t bytes_succ = f_info->read ? f_info->read(buf, cur_offset, bytes_to_read)
                                     : ramdisk_read(buf, cur_offset, bytes_to_read);
    f_info->open_offset += bytes_succ;
    return bytes_succ;
  } else {
    assert(f_info->read);
    return f_info->read(buf, 0, len);
  }
}

size_t fs_write(int fd, const void *buf, size_t len) {
  Finfo* f_info = find_finfo(fd);
  if (is_block_dev(f_info)) {
    size_t cur_offset = f_info->disk_offset + f_info->open_offset;
    size_t remain_bytes = f_info->size - f_info->open_offset;
    size_t bytes_to_write = len > remain_bytes ? remain_bytes : len;
    size_t bytes_succ = f_info->write ? f_info->write(buf, cur_offset, bytes_to_write)
                                      : ramdisk_write(buf, cur_offset, bytes_to_write);
    f_info->open_offset += bytes_succ;
    return bytes_succ;
  } else {
    assert(f_info->write);
    return f_info->write(buf, 0, len);
  }
}

size_t fs_lseek(int fd, size_t offset, int whence) {
  Finfo* f_info = find_finfo(fd);
  assert(is_block_dev(f_info));
  switch (whence) {
  case SEEK_CUR:
    f_info->open_offset += offset;
    break;
  case SEEK_SET:
    f_info->open_offset = offset;
    break;
  case SEEK_END:
    f_info->open_offset = f_info->size + offset;
    break;
  default:
    panic("Invalid lseek whence: %d", whence);
    break;
  }
  assert(f_info->open_offset <= f_info->size);
  return f_info->open_offset;
}

int fs_close(int fd) {
  return 0;
}
