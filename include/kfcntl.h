#ifndef _KFCNTL_H
#define _KFCNTL_H

#include <types.h>
#include <libio.h>

struct _file_descriptor;

typedef struct _open_file_descriptor {
	uint32_t flags;
	uint8_t buffer[512];
	uint32_t current_octet_buf;
	uint32_t current_cluster;
	uint32_t current_octet;
	uint32_t first_cluster;
	uint32_t file_size;
	void * extra_data;
	size_t (*write)(struct _open_file_descriptor *, const void*, size_t);
	size_t (*read)(struct _open_file_descriptor *,void*, size_t);
} open_file_descriptor;

/** 
 * @brief Initialise les descripteurs de fichiers standards.
 * 
 *	Initialise les descripteurs de fichiers standards.
 *
 * @param fd0 stdin
 * @param fd1 stdout
 * @param fd2 stderr
 */
void init_stdfd(struct _file_descriptor *fd0, struct _file_descriptor *fd1, struct _file_descriptor *fd2);

void* sys_open(uint32_t p_ofd, uint32_t zero1, uint32_t zero2);

#endif
