/**
 * @file unistd.c
 *
 * @author TacOS developers 
 *
 * @section LICENSE
 *
 * Copyright (C) 2010-2014 TacOS developers.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 3 of
 * the License, or (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details at
 * http://www.gnu.org/copyleft/gpl.html
 *
 * You should have received a copy of the GNU General Public License 
 * along with this program; if not, see <http://www.gnu.org/licenses>.
 *
 * @section DESCRIPTION
 *
 * Description de ce que fait le fichier
 */

#include <dirent.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/syscall.h>
#include <unistd.h>

int chdir(const char *path) {
	int ret;
	syscall(SYS_CHDIR, (uint32_t) path, (uint32_t) &ret, 0);

	if (ret == 0) {
		char *cwd = getcwd(NULL, 0);
		char *dest = malloc(5 + strlen(cwd));
		sprintf(dest, "PWD=%s", cwd);
		putenv(dest);
	}

	return ret;
}

char *get_current_dir_name(void) {
	// Utilise PWD si existe et correct.
	
	char * pwd = getenv("PWD");
	struct stat pwdstat;

	if (pwd != NULL && stat(pwd, &pwdstat) == 0) {
		return strdup(pwd);
	}

	return getcwd(NULL, 0);
}

char * getcwd(char * buf, size_t size) {
	int need_free = 0;

	if (size == 0 && buf != NULL) {
		errno = EINVAL;
		goto err;
	}

	if (buf == NULL) {
		if (size == 0) {
			size = 1024;
		}
		buf = malloc(size);
		need_free = 1;
	}

	ssize_t s = readlink("/proc/self/cwd", buf, size);
	if (s == -1) {
		goto err;
	}

	if (size && size <= (size_t) s) {
		errno = ERANGE;
		goto err;
	}
	buf[s] = '\0';

	return buf;

err:
	if (need_free) {
		free(buf);
	}
	return NULL;
}

pid_t getpid(void) {
	pid_t pid;
	syscall(SYS_GETPID, (uint32_t)&pid, 0, 0);
	return pid;
}

pid_t getppid(void) {
	pid_t ppid;
	syscall(SYS_GETPPID, (uint32_t)&ppid, 0, 0);
	return ppid;
}

unsigned int sleep(unsigned int seconds)
{
	return usleep(1000000*seconds);
}

unsigned int usleep(unsigned int microseconds)
{
	syscall(SYS_SLEEP, microseconds, 0, 0);
	return 0;
}

ssize_t write(int fd, const void *buf, size_t count) {
	if (count > 0) {
		syscall(SYS_WRITE, fd, (uint32_t) buf, (uint32_t)(&count));
	}

	return count;
}

ssize_t read(int fd, void *buf, size_t count) {
	if (count > 0) {
		syscall(SYS_READ, fd, (uint32_t) buf, (uint32_t)(&count));
	}

	return count;
}

int lseek(int fd, long offset, int whence) {
	syscall(SYS_SEEK, fd, (uint32_t)(&offset), (uint32_t)(whence));

	return offset;
}

int chmod(const char *path, mode_t mode) {
	int ret;
	syscall(SYS_CHMOD, (uint32_t)path, (uint32_t)mode, (uint32_t)&ret);
	return ret;
}

int chown(const char *path, uid_t owner, gid_t group) {
	int *ret = (int*)&group;
	syscall(SYS_CHMOD, (uint32_t)path, (uint32_t)owner, (uint32_t)ret);
	return *ret;
}

int stat(const char *path, struct stat *buf) {
	int ret = 1; // follow link
	syscall(SYS_STAT, (uint32_t)path, (uint32_t)buf, (uint32_t)&ret);
	return ret;
}

int lstat(const char *path, struct stat *buf) {
	int ret = 0; // don't follow link
	syscall(SYS_STAT, (uint32_t)path, (uint32_t)buf, (uint32_t)&ret);
	return ret;
}

int symlink(const char *target, const char *linkpath) {
	int ret;
	syscall(SYS_SYMLINK, (uint32_t)target, (uint32_t)linkpath, (uint32_t)&ret);
	return ret;
}

int mknod(const char *path, mode_t mode, dev_t dev) {
	syscall(SYS_MKNOD, (uint32_t)path, (uint32_t)mode, (uint32_t)&dev);
	return (int)dev;
}

int unlink(const char *path) {
	int ret;
	syscall(SYS_UNLINK, (uint32_t)path, (uint32_t)&ret, 0);
	return ret;
}

int rmdir(const char *path) {
	int ret;
	syscall(SYS_RMDIR, (uint32_t)path, (uint32_t)&ret, 0);
	return ret;
}

int dup(int oldfd) {
	int newfd;
	syscall(SYS_DUP, (uint32_t) oldfd, (uint32_t)&newfd, 0);
	return newfd;
}

int dup2(int oldfd, int newfd) {
	syscall(SYS_DUP2, (uint32_t) oldfd, (uint32_t)&newfd, 0);
	return newfd;
}

int gethostname(char *name, size_t len) {
	strncpy(name, "tacos", len);
	return 0;
}

int isatty(int fd) {
	// TODO !
	fprintf(stderr, "isatty non implémenté ! %d\n", fd);
	return 1;
}

int execv(const char *path __attribute__ ((unused)), char *const argv[]) {
	char *cmd = malloc(1024);
	cmd[0] = '\0';
	int i = 0;
	while (argv[i] != NULL) {
		strcat(cmd, argv[i]);
		i++;
	}
	return exec_elf(cmd);
}

int execvp(const char *file, char *const argv[]) {
	if (argv[0][0] == '/') {
		return execv(file, argv);
	} else {
		char *cmd = malloc(1024);
		char *path = getenv("PATH");
		sprintf(cmd, "%s/%s", path, argv[0]);
		int i = 1;
		while (argv[i] != NULL) {
			strcat(cmd, argv[i]);
			i++;
		}
		return exec_elf(cmd);
	}
}

ssize_t readlink(const char *path, char *buf, size_t bufsize) {
	ssize_t *ret = (ssize_t*)&bufsize;
	syscall(SYS_READLINK, (uint32_t) path, (uint32_t)buf, (uint32_t)ret);
	return *ret;
}
