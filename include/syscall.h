#ifndef _SYSCALL_H_
#define _SYSCALL_H_

#define SYSCALL_ID_READ 0x0

void syscall_init(void);
unsigned long long syscall(unsigned long long syscall_id,
                           unsigned long long arg1, unsigned long long arg2,
                           unsigned long long arg3);
unsigned long long read(unsigned long long fd, void *buf,
                        unsigned long long buf_size);

unsigned long long read_from_stdin(char *buf, unsigned long long buf_size);

#endif
