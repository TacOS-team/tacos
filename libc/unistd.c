/**
 * @file unistd.c
 *
 * @author TacOS developers 
 *
 * @section LICENSE
 *
 * Copyright (C) 2010, 2011, 2012 - TacOS developers.
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

char * get_absolute_path(const char *dirname) {
	char * cwd = getenv("PWD");
	if (cwd == NULL) {
		cwd = "/";
	}

	int lencwd = strlen(cwd);
	if (lencwd <= 1) {
		lencwd = 0;
	}
	char *abs = malloc(lencwd + strlen(dirname) + 2);
	strcpy(abs, cwd);
	abs[lencwd] = '/';
	strcpy(abs + lencwd + 1, dirname);
	return abs;
}

int chdir(const char *path) {
	char * cwd = getenv("PWD");
	if (cwd == NULL) {
		putenv("PWD=/");
		cwd = "/";
	}

	DIR *dir;
	//XXX: Je l'ai mis ici mais vu que ".." est dispo dans le fs, je devrais 
	//     pouvoir le mettre dans le if du opendir.
	if (strcmp(path, "..") == 0 && strcmp(cwd, "/") != 0) {
		char *r = strrchr(cwd, '/');
		if (r == cwd) {
			r[1] = '\0';
		} else {
			*r = '\0';
		}		
		char *dest = malloc(5 + strlen(cwd));
		sprintf(dest, "PWD=%s", cwd);
		putenv(dest);
		return 0;
	}

	if ((dir = opendir(path)) != NULL) {
		if (path[0] == '/') {
			cwd = (char*)path;
		} else {
			cwd = get_absolute_path(path);
		}
		int len = strlen(cwd);
		if (len > 1 && cwd[len-1] == '/') {
			cwd[len-1] = '\0';
			len--;
		}
		char *dest = malloc(5 + len);
		sprintf(dest, "PWD=%s", cwd);
		putenv(dest);
		return 0;
	}
	return 1;
}

const char * getcwd(char * buf, size_t size) {
	char * cwd = getenv("PWD");
	if (cwd == NULL) {
		cwd = "/";
	}

	if (buf == NULL) {
		if (size == 0) {
			return strdup(cwd);
		} else {
			buf = malloc(size);		
		}
	}

	int i = 0;
	while (cwd[i] != '\0') {
		size--;
		if (size <= 0) {
			errno = ERANGE;
			return NULL;
		}
		buf[i] = cwd[i];
		i++;
	}
	buf[i] = '\0';

	return buf;
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

int stat(const char *path, struct stat *buf) {
	int ret;
	if (path[0] != '/') {
		char * absolutepath = get_absolute_path(path);
		syscall(SYS_STAT, (uint32_t)absolutepath, (uint32_t)buf, (uint32_t)&ret);
		free(absolutepath);
	} else {
		syscall(SYS_STAT, (uint32_t)path, (uint32_t)buf, (uint32_t)&ret);
	}
	return ret;
}

int mknod(const char *path, mode_t mode, dev_t dev) {
	if (path[0] != '/') {
		char * absolutepath = get_absolute_path(path);
		syscall(SYS_MKNOD, (uint32_t)absolutepath, (uint32_t)mode, (uint32_t)&dev);
		free(absolutepath);
	} else {
		syscall(SYS_MKNOD, (uint32_t)path, (uint32_t)mode, (uint32_t)&dev);
	}
	return (int)dev;
}

int unlink(const char *path) {
	int ret;
	if (path[0] != '/') {
		char * absolutepath = get_absolute_path(path);
		syscall(SYS_UNLINK, (uint32_t)absolutepath, (uint32_t)&ret, 0);
		free(absolutepath);
	} else {
		syscall(SYS_UNLINK, (uint32_t)path, (uint32_t)&ret, 0);
	}
	return ret;
}

int rmdir(const char *path) {
	int ret;
	if (path[0] != '/') {
		char * absolutepath = get_absolute_path(path);
		syscall(SYS_RMDIR, (uint32_t)absolutepath, (uint32_t)&ret, 0);
		free(absolutepath);
	} else {
		syscall(SYS_RMDIR, (uint32_t)path, (uint32_t)&ret, 0);
	}
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
	return 0;
}
