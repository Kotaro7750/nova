#include "include/common.h"

int strncmp(char *str1, char *str2, unsigned int size) {
  for (int i = 0; i < size; i++) {
    if (str1[i] != str2[i]) {
      return 1;
    }
  }
  return 0;
}
