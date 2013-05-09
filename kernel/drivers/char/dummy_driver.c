#include <fs/devfs.h>
#include <types.h>
#include <klog.h>

static ssize_t dummy_read(open_file_descriptor* ofd __attribute__((unused)), void* buf __attribute__((unused)), size_t count) 
{
	klog("reading %d bytes on dummy_device.",count);
	return 0;
}

static ssize_t dummy_write(open_file_descriptor* ofd __attribute__((unused)), const void* buf, size_t count)
{
	klog("writing \"%s\" on dummy device.", buf);
	return count;
}

static chardev_interfaces di = {
	.read = dummy_read,
	.write = dummy_write,
	.open = NULL,
	.close = NULL,
	.ioctl = NULL
};

void init_dummy()
{
	klog("initializating dummy...");
	if(register_chardev("dummy", &di) != 0)
		kerr("error registering dummy driver.");
}

/*

size_t (*write)(open_file_descriptor*, const char*, size_t);
int (*seek) (open_file_descriptor *, long, int);
int (*ioctl) (open_file_descriptor*, unsigned int, unsigned long);
int (*open) (open_file_descriptor);
int (*close) (open_file_descriptor*);
*/
