#ifndef _DEVFS_H
#define _DEVFS_H

#include <types.h>
#include <kfcntl.h>

typedef struct {
	size_t (*read)(open_file_descriptor *,void*, size_t);
	size_t (*write)(open_file_descriptor *, const void*, size_t);
	int (*seek) (open_file_descriptor *, long, int);
	int (*ioctl) (open_file_descriptor*, unsigned int, void*);
	int (*open) (open_file_descriptor*);
	int (*close) (open_file_descriptor*);
}driver_interfaces;

void devfs_init();
int register_driver(const char* name, driver_interfaces* di);

#endif /* _DEVFS_H */
