#include <fd_types.h>
#include <types.h>
#include <klog.h>
#include <ksem.h>
#include <kmalloc.h>
#include <vfs.h>

static int pipe_open(open_file_descriptor * ofd);
static ssize_t pipe_read(struct _open_file_descriptor *ofd, void* buf, size_t size);
static ssize_t pipe_write(struct _open_file_descriptor *ofd, const void* buf, size_t size);

struct _open_file_operations_t pipe_fops = {.write = pipe_write, .read = pipe_read, .seek = NULL, .ioctl = NULL, .open = pipe_open, .close = NULL, .readdir = NULL};

#define DATA_SIZE 4096

struct extra_data_pipe_t {
	int sem_open;
	int sem_read;
	int sem_write;
 // On se sert de data comme un buffer circulaire.
	uint8_t data[DATA_SIZE]; // Idéalement il lui faudrait sa propre page pour des raisons de perf.
	int pos_read; // position prochaine lecture.
	int pos_write; // position prochaine écriture.
};

static ssize_t pipe_read(struct _open_file_descriptor *ofd, void* buf, size_t size) {
	klog("pipe read");

	struct extra_data_pipe_t *extra = (struct extra_data_pipe_t*)ofd->dentry->d_inode->i_fs_specific;

	if (extra->pos_read == extra->pos_write) {
		ksemP(extra->sem_read);
	}

	size_t c = 0;
	while (extra->pos_read != extra->pos_write && c < size) {
		((uint8_t*)buf)[c] = extra->data[extra->pos_read];
		c++;
		extra->pos_read = (extra->pos_read + 1) % DATA_SIZE;
	}

	if (c > 0) {
		ksemV(extra->sem_write);
	}
// TODO: ajouter caractère \0 ?
	kprintf("read ret %d %s\n", c, buf);
	return c;
}


static ssize_t pipe_write(struct _open_file_descriptor *ofd, const void* buf, size_t size) {
	klog("pipe write");

	struct extra_data_pipe_t *extra = (struct extra_data_pipe_t*)ofd->dentry->d_inode->i_fs_specific;

	if (extra->pos_read == extra->pos_write + 1) {
		ksemP(extra->sem_write);
	}

	size_t c = 0;
	while (extra->pos_read != extra->pos_write + 1 && c < size) {
		extra->data[extra->pos_write] = ((uint8_t*)buf)[c];
		c++;
		extra->pos_write = (extra->pos_write + 1) % DATA_SIZE;
	}

	if (c > 0) {
		ksemV(extra->sem_read);
	}
	
	kprintf("ret %d\n", c);
	return c;
}

static int pipe_open(open_file_descriptor * ofd) {
	klog("pipe open.");
	kprintf("%d\n", ofd->flags);

	ofd->extra_data = NULL;

	if (ofd->dentry->d_inode->i_count == 1) {
		klog("first open !");
		struct extra_data_pipe_t *extra = kmalloc(sizeof(struct extra_data_pipe_t));
		extra->sem_open = ksemget(SEM_NEW, SEM_CREATE);
		int val = 0;
		ksemctl(extra->sem_open, SEM_SET, &val);

		extra->pos_read = extra->pos_write = 0;
		extra->sem_read = ksemget(SEM_NEW, SEM_CREATE);
		val = 0;
		ksemctl(extra->sem_read, SEM_SET, &val);
		extra->sem_write = ksemget(SEM_NEW, SEM_CREATE);
		val = 1;
		ksemctl(extra->sem_write, SEM_SET, &val);

		ofd->dentry->d_inode->i_fs_specific = (void*)extra;

		ksemP(extra->sem_open);
	} else {
		struct extra_data_pipe_t *extra = (struct extra_data_pipe_t*)ofd->dentry->d_inode->i_fs_specific;
		ksemV(extra->sem_open);
		// Attention, vérifier qu'on a un lecteur et un écrivain et pas juste 2 read ou 2 write !
	}
	
	return 0;
}
