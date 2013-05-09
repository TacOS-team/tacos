#include <fd_types.h>
#include <types.h>
#include <pipe.h>
#include <klog.h>

struct _open_file_operations_t pipe_fops = {.write = NULL, .read = NULL, .seek = NULL, .ioctl = NULL, .open = pipe_open, .close = NULL, .readdir = NULL};

int pipe_open(open_file_descriptor * ofd) {
	klog("pipe open.");
}
