#include "include/common.h"
#include "include/fbcon.h"
#include "include/lock.h"

void strncp(char *src, char *dst, int size) {
  for (int i = 0; i < size; i++) {
    dst[i] = src[i];
  }
}
int strncmp(char *str1, char *str2) {
  int i = 0;
  while (str1[i] == str2[i]) {
    if (str1[i] == '\0') {
      return 0;
    }
    i++;
  }
  return 1;
}

void queue_init(struct queue *q) {
  q->status = OK;
  q->size = 0;
  q->start = 0;
  q->end = 0;
}

void enqueue(struct queue *q, unsigned char data) {
  char ifflag;
  if (q->size == BUF_SIZE) {
    q->status = ERROR;
    return;
  }
  q->status = OK;

  intr_lock(&ifflag);

  q->buf[q->end] = data;
  q->end = q->end++ % BUF_SIZE;
  q->size++;

  intr_unlock(&ifflag);
}

unsigned char dequeue(struct queue *q) {
  char ifflag;
  if (q->size == 0) {
    q->status = ERROR;
    return NULL;
  }
  q->status = OK;

  intr_lock(&ifflag);

  unsigned char data = q->buf[q->start];
  q->start = q->start++ % BUF_SIZE;
  q->size--;

  intr_unlock(&ifflag);
  return data;
}
