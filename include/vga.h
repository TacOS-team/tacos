#ifndef _VGA_H_
#define _VGA_H_

#include <ksyscall.h>
#include <vga_types.h>

void VGA_set_mode(enum VGA_mode mode);
void VGA_write_buf(char *buffer);

SYSCALL_HANDLER1(sys_vgasetmode, enum VGA_mode mode);
SYSCALL_HANDLER1(sys_vgawritebuf, char *buffer);

#endif
