/**
 * @file stat.c
 *
 * @author TacOS developers 
 *
 *
 * @section LICENSE
 *
 * Copyright (C) 2010 - TacOS developers.
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
#include <unistd.h>
#include <sys/stat.h>

int main(int argc, char** argv) {
	int i;
	for(i = 1; i < argc; i++) {
		struct stat buf;
		if (stat(argv[i], &buf) == 0) {
			printf("  File: '%s'\n", argv[i]);
			printf("  Size: %d\n", buf.st_size);
			printf(" Inode: %d\n", buf.st_ino);
			printf("Access: %o		uid: %d		gid: %d\n", buf.st_mode, buf.st_uid, buf.st_gid);
			printf("Modify: %d\n", buf.st_mtime);
			printf("Change: %d\n", buf.st_ctime);
		} else {
			printf("%s: file not found.\n", argv[i]);
		}	
	}
	return 0;
}
