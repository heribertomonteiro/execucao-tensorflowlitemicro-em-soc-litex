#include <sys/stat.h>
#include <stdint.h>

int _close(int fd) { (void)fd; return -1; }
int _fstat(int fd, struct stat *st) { (void)fd; st->st_mode = S_IFCHR; return 0; }
int _isatty(int fd) { (void)fd; return 1; }
int _lseek(int fd, int offset, int whence) { (void)fd; (void)offset; (void)whence; return -1; }
int _read(int fd, void *buf, unsigned int count) { (void)fd; (void)buf; (void)count; return 0; }
int _write(int fd, const void *buf, unsigned int count) { (void)fd; (void)buf; return (int)count; }
int _kill(int pid, int sig) { (void)pid; (void)sig; return -1; }
int _getpid(void) { return 1; }
void _exit(int status) { (void)status; while (1) { __asm__ volatile ("wfi"); } }
