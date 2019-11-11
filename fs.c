#include "include/fs.h"
#include "include/common.h"

struct file *fs_start;

void fs_init(void *_fs_start) { fs_start = _fs_start; }

struct file *open(char *filename) {
  struct file *file = fs_start;

  while (file->name[0] != '0x0') {
    if (!strncmp(filename, file->name)) {
      return file;
    }
    file = (struct file *)(file->data + file->size);
  }
  return NULL;
}
