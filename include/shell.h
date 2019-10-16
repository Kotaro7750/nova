#ifndef _SHELL_H_
#define _SHELL_H_

#define MAX_INPUT 255

extern char input_buffer[MAX_INPUT];
void shsh(void);
void input_to_buffer(char c);

#endif
