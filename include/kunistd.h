#ifndef _KUNISTD_H
#define _KUNISTD_H

#include <types.h>

void sys_write(uint32_t fd, uint32_t p_buf, uint32_t count);
void sys_read(uint32_t fd, uint32_t p_buf, uint32_t count);
void sys_seek(uint32_t fd, uint32_t p_offset, uint32_t p_whence);

#endif
