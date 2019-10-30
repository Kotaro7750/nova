#ifndef _COMMON_H_
#define _COMMON_H_

#define NULL 0x0
#define BUF_SIZE 256

int strncmp(char *str1, char *str2, unsigned size);

enum QUEUE_STATUS { OK, ERROR };

struct queue {
  enum QUEUE_STATUS status;
  unsigned char buf[BUF_SIZE];
  unsigned char start, end;
  int size;
};

void enqueue(struct queue *queue, unsigned char data);
unsigned char dequeue(struct queue *q);
void queue_init(struct queue *q);

#endif
