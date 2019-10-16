#include "include/shell.h"
#include "include/fbcon.h"

char input_buffer[MAX_INPUT] = {'\0'};
int get_input(char *input);
void clear_input_buffer(void);
//キーボード割り込みで入力されたキーコードをどこかに保存
// shshは逐一それを取得
void shsh(void) {
  while (1) {
    char input[MAX_INPUT];
    int n = get_input(input);

    if (n == 0) {
      continue;
    }

    puts(input);
    clear_input_buffer();
  }
}

int get_input(char *input) {
  int num = 0;
  for (int i = 0; i < MAX_INPUT; i++) {
    input[i] = input_buffer[i];
    if (input[i] == '\0') {
      break;
    }
    num++;
  }
  return num;
}

void input_to_buffer(char c) {
  for (int i = 0; i < MAX_INPUT - 1; i++) {
    if (input_buffer[i] == '\0') {
      input_buffer[i] = c;
      input_buffer[i + 1] = '\0';
    }
    return;
  }
  return;
}

void clear_input_buffer(void) {
  for (int i = 0; i < MAX_INPUT; i++) {
    input_buffer[i] = '\0';
  }
  return;
}
