/**
 * @file stat.c
 *
 * @author TacOS developers 
 *
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
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <time.h>
#include <unistd.h>


static char* get_filetype(mode_t mode) {
	if (S_ISBLK(mode)) {
		return "block device";
	} else if (S_ISCHR(mode)) {
		return "char device";
	} else if (S_ISDIR(mode)) {
		return "directory";
	} else if (S_ISFIFO(mode)) {
		return "FIFO";
	} else if (S_ISLNK(mode)) {
		return "symbolic link";
	} else if (S_ISREG(mode)) {
		return "regular file";
	} else if (S_ISSOCK(mode)) {
		return "socket";
	} else {
		return "type non reconnu";
	}
}

char* disp_mode(mode_t mode) {
	char *m = strdup("----------");
	if (S_ISDIR(mode)) {
		m[0] = 'd';
	} else if (S_ISLNK(mode)) {
		m[0] = 'l';
	} else if (S_ISFIFO(mode)) {
		m[0] = 'p';
	} else if (S_ISSOCK(mode)) {
		m[0] = 's';
	}

	if (mode & S_IRUSR) {
		m[1] = 'r';
	}
	if (mode & S_IWUSR) {
		m[2] = 'w';
	}
	if (mode & S_IXUSR) {
		m[3] = 'x';
	}

	if (mode & S_IRGRP) {
		m[4] = 'r';
	}
	if (mode & S_IWGRP) {
		m[5] = 'w';
	}
	if (mode & S_IXGRP) {
		m[6] = 'x';
	}
	
	if (mode & S_IROTH) {
		m[7] = 'r';
	}
	if (mode & S_IWOTH) {
		m[8] = 'w';
	}
	if (mode & S_IXOTH) {
		m[9] = 'x';
	}

	return m;
}


int main(int argc, char** argv) {
	int i;
	for(i = 1; i < argc; i++) {
		struct stat buf;
		if (lstat(argv[i], &buf) == 0) {
			if (S_ISLNK(buf.st_mode)) {
				char link[80];
				readlink(argv[i], link, sizeof(link));
				printf("  File: '%s' -> '%s'\n", argv[i], link);
			} else {
				printf("  File: '%s'\n", argv[i]);
			}
			printf("  Size: %d\n", (int) buf.st_size);
			printf(" Inode: %u\n", (unsigned int) buf.st_ino);
			printf("  Type: %s\n", get_filetype(buf.st_mode));
			char *mode = disp_mode(buf.st_mode);
			printf("Access: (%o/%s)		uid: %d		gid: %d\n", buf.st_mode & ~S_IFMT, mode, buf.st_uid, buf.st_gid);
			free(mode);
			printf("Modify: %s", ctime(&buf.st_mtime));
			printf("Change: %s", ctime(&buf.st_ctime));
		} else {
			printf("%s: file not found.\n", argv[i]);
		}	
	}
	return 0;
}
