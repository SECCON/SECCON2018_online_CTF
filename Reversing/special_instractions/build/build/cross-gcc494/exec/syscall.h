#ifndef	_SYSCALL_H_INCLUDED_
#define	_SYSCALL_H_INCLUDED_

void __exit(int status) __attribute__((noreturn));
int __read(int fd, void *buffer, int size);
int __write(int fd, const void *buffer, int size);
int __open(const char *path, int flags, int mode);
int __close(int fd);

#ifdef USE_SYSCALL_WRAPPER
void __r_exit(int dummy, int status) __attribute__((noreturn));
int __r_read(int dummy, int fd, void *buffer, int size);
int __r_write(int dummy, int fd, const void *buffer, int size);
int __r_open(int dummy, const char *path, int flags, int mode);
int __r_close(int dummy, int fd);
#endif

#endif
