/**
 * @file fcntl.c
 */

#include <types.h>
#include <fcntl.h>
#include <syscall.h>

int open(const char *pathname, int flags) {
	int id;
	syscall(SYS_OPEN,(uint32_t)&id,(paddr_t) pathname,flags);
	
	return id;
}
