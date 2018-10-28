#ifndef UTIL_H
#define UTIL_H

#define TIMEOUT 5

void __stack_chk_fail(void);
void signal_handler_timeout(int sig);
void ctf_setup(void);
void open_read_write(char *filename);

#endif /* UTIL_H */
