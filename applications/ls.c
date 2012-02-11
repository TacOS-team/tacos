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
 * @brief List files
 */

#include <dirent.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <time.h>
#include <unistd.h>

#define MAX_FILES 1024

static const char mon_name[12][4] = {
          "Jan", "Feb", "Mar", "Apr", "May", "Jun",
          "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"
     };

struct res_entry {
	char name[256];
	struct stat s;
};

static struct res_entry *entries[MAX_FILES];
static int n_entries;
static int long_format = 0;
static int disp_all = 0;
static int disp_classify = 0;
static int human = 0;

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
	return '\0';
}

void convert_human(size_t size, size_t *nsize, char *c) {
	int p = 0;
	while (size > 1024) {
		size /= 1024;
		p++;
	}

	switch (p) {
		case 0: *c = ' '; break;
		case 1:	*c = 'K'; break;
		case 2:	*c = 'M'; break;
		case 3:	*c = 'G'; break;
		case 4:	*c = 'T'; break;
		case 5: *c = 'P'; break;
	}
	*nsize = size;
}

static int compar(const void *p1, const void *p2) {
	struct res_entry **a1 = (struct res_entry**) p1;
	struct res_entry **a2 = (struct res_entry**) p2;

	return strcasecmp((*a1)->name, (*a2)->name);
}

void listdir(const char *path) {
	struct stat buf;
	DIR* dir = opendir(path);
	struct dirent* entry;
	size_t pathlen = strlen(path);
	char *filepath = malloc(255 + pathlen + 1);
	strcpy(filepath, path);
	if (pathlen > 0 && path[pathlen - 1] != '/') {
		filepath[pathlen] = '/';
		pathlen++;
	}

	if (dir != NULL) {
		while((entry = readdir(dir))) {
			if (entry->d_name[0] == '.' && !disp_all) {
				continue;
			}
			strcpy(filepath + pathlen, entry->d_name);
			if (stat(filepath, &buf) == 0) {
				add_entry(entry->d_name, &buf);
			}
		}
		closedir(dir);

		// TODO: do sort.
		qsort(entries, n_entries, sizeof(struct res_entry*), compar);

		int i = 0;
		while (entries[i]) {
			if (long_format) {
				// mode
				disp_mode(entries[i]->s.st_mode);

				// user + group
				printf(" %d %d", entries[i]->s.st_uid, entries[i]->s.st_gid);

				// taille
				if (human) {
					char c;
					size_t size;
					convert_human(entries[i]->s.st_size, &size, &c);
					printf(" %d%c", size, c);
				} else {
					printf(" %d", entries[i]->s.st_size);
				}
				
				// date
				time_t n = time(NULL);
				time_t d = entries[i]->s.st_mtime;
				struct tm *t = localtime(&d);
				printf(" %s %d", mon_name[t->tm_mon], t->tm_mday);
				if (n - d > 366 * 24 * 60 * 60) {
					printf("  %d", t->tm_year + 1900);
				} else {
					printf(" %d:%d", t->tm_hour, t->tm_min);
				}

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

	free(filepath);
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
				case 'h': human = 1; break;
			}
			i++;
		}
		marg++;
	}

	if (argc == marg) {
		listdir(getcwd(NULL, 0));
	} else {
		int i;
		for(i = marg; i < argc; i++) {
			list(argv[i]);
		}
	}
	printf("\n");
	return 0;
}

