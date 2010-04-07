#ifndef UNISTD_H
#define UNISTD_H

#include <types.h>

unsigned int sleep(unsigned int seconds);
unsigned int usleep(unsigned int milliseconds);

size_t write(int fd, const void *buf, size_t count);

void* sys_write(uint32_t fd, uint32_t p_buf, uint32_t count);

#endif //UNISTD_H
