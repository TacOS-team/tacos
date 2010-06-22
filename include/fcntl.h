#ifndef _FCNTL_H_
#define _FCNTL_H_

/**
 * @file fcntl.h
 */

#include <types.h>
#include <libio.h>

// Define pour open.

#define O_ACCMODE 00000003
#define O_RDONLY  00000000
#define O_WRONLY  00000001
#define O_RDWR    00000002
#ifndef O_CREAT
#define O_CREAT      00000100 /* not fcntl */
#endif
#ifndef O_EXCL
#define O_EXCL    00000200 /* not fcntl */
#endif
#ifndef O_NOCTTY
#define O_NOCTTY  00000400 /* not fcntl */
#endif
#ifndef O_TRUNC
#define O_TRUNC      00001000 /* not fcntl */
#endif
#ifndef O_APPEND
#define O_APPEND  00002000
#endif
#ifndef O_NONBLOCK
#define O_NONBLOCK   00004000
#endif
#ifndef O_SYNC
#define O_SYNC    00010000
#endif
#ifndef FASYNC
#define FASYNC    00020000 /* fcntl, for BSD compatibility */
#endif
#ifndef O_DIRECT
#define O_DIRECT  00040000 /* direct disk access hint */
#endif
#ifndef O_LARGEFILE
#define O_LARGEFILE  00100000
#endif
#ifndef O_DIRECTORY
#define O_DIRECTORY  00200000 /* must be a directory */
#endif
#ifndef O_NOFOLLOW
#define O_NOFOLLOW   00400000 /* don't follow links */
#endif
#ifndef O_NOATIME
#define O_NOATIME 01000000
#endif
#ifndef O_CLOEXEC
#define O_CLOEXEC 02000000 /* set close_on_exec */
#endif
#ifndef O_NDELAY
#define O_NDELAY  O_NONBLOCK
#endif

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

int open(const char *pathname, int flags);
//int open(const char *pathname, int flags, mode_t mode);

//int creat(const char *pathname, mode_t mode);

void* sys_open(uint32_t p_ofd, uint32_t zero1, uint32_t zero2);

#endif
