/**
 * @file ls.c
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
 * List files
 */

#include <dirent.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <string.h>

#define MAX_FILES 1024

struct res_entry {
	char name[256];
	struct stat s;
};

static struct res_entry *entries[MAX_FILES];
static int n_entries;
static int long_format = 0;
static int disp_all = 0;
static int disp_classify = 0;

void add_entry(char *name, struct stat *s) {
	entries[n_entries] = malloc(sizeof(struct res_entry));
	strcpy(entries[n_entries]->name, name);
	entries[n_entries]->s = *s;
	n_entries++;
}

void disp_mode(mode_t mode) {
	char m[11];
	strcpy(m,	"----------");
	if (S_ISDIR(mode)) {
		m[0] = 'd';
	} else if (S_ISLNK(mode)) {
		m[0] = 'l';
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

	printf("%s", m);
}

char classify(mode_t mode) {
	if (S_ISDIR(mode)) {
		return '/';
	}
	if (S_ISLNK(mode)) {
		return '@';
	}
	if (S_ISFIFO(mode)) {
		return '|';
	}
	if (S_ISSOCK(mode)) {
		return '=';
	}
	if (mode & S_IXUSR || mode & S_IXGRP || mode & S_IXOTH) {
		return '*';
	}
	return ' ';
}

void listdir(const char *path) {
	struct stat buf;
	DIR* dir = opendir(path);
	struct dirent* entry;
	if (dir != NULL) {
		while((entry = readdir(dir))) {
			if (entry->d_name[0] == '.' && !disp_all) {
				continue;
			}
			stat(entry->d_name, &buf);
			add_entry(entry->d_name, &buf);
		}
		closedir(dir);

		// TODO: do sort.

		int i = 0;
		while (entries[i]) {
			if (long_format) {
				// mode
				disp_mode(entries[i]->s.st_mode);
				// TODO: date

				if (disp_classify) {
					printf(" %s%c\n", entries[i]->name, classify(entries[i]->s.st_mode));
				} else {
					printf(" %s\n", entries[i]->name);
				}
			} else {
				if (disp_classify) {
					printf("%s%c ", entries[i]->name, classify(entries[i]->s.st_mode));
				} else {
					printf("%s ", entries[i]->name);
				}
			}
			i++;
		}
	} else {
		fprintf(stderr, "%s not found.\n", path);
	}
}

void list(const char *path) {
	struct stat buf;
	if (stat(path, &buf) == 0) {
		if (S_ISDIR(buf.st_mode)) {
			listdir(path);
		} else {
			printf("%s ", path);
		}
	} else {
		fprintf(stderr, "%s not found.\n", path);
	}
}

int main(int argc, char** argv)
{
	int marg = 1;
	if (argc > 1 && argv[1][0] == '-') {
		int i = 1;
		while (argv[1][i] != '\0') {
			switch (argv[1][i]) {
				case 'l': long_format = 1; break;
				case 'a': disp_all = 1; break;
				case 'F': disp_classify = 1; break;
			}
			i++;
		}
		marg++;
	}

	if (argc == marg) {
		list(getcwd(NULL, 0));
	} else {
		int i;
		for(i = marg; i < argc; i++) {
			list(argv[i]);
		}
	}
	printf("\n");
	return 0;
}

