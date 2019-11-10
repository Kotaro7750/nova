#include "include/fs.h"
#include "include/common.h"

struct file *fs_start;

void fs_init(void *_fs_start) { fs_start = _fs_start; }

struct file *open(char *filename) {
  struct file *file = fs_start;
  while (*(char *)file != '\0') {
    if (strncmp()) {
      return file;
    }
  }
  return NULL;
}
