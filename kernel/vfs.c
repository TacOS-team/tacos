#include <vfs.h>
#include <kfat.h>
#include <kmalloc.h>
//XXX: libc
#include <string.h>

typedef struct _available_fs_t {
	file_system_t *fs;
	struct _available_fs_t *next;
} available_fs_t;

static available_fs_t *fs_list;

typedef struct _mounted_fs_t {
	fs_instance_t *instance;
	char *name;
	struct _mounted_fs_t *next;
} mounted_fs_t;

static mounted_fs_t *mount_list;

static void vfs_register_fs(file_system_t *fs) {
	available_fs_t *element = kmalloc(sizeof(available_fs_t));
	element->fs = fs;
	element->next = fs_list;
	fs_list = element;
}

void vfs_init() {
	file_system_t *fs = kmalloc(sizeof(file_system_t));
	fs->name = "FAT";
	fs->mount = mount_FAT;
	fs->umount = umount_FAT;
	vfs_register_fs(fs);
}

static fs_instance_t* get_instance_from_path(const char * pathname, int *len) {
	char *mountpoint;
	int i = 1;
	while (pathname[i] != '/' && pathname[i] != '\0') {
		i++;
	}
	if (i <= 1) {
		*len = 0;
		return NULL;
	}
	*len = i;
	mountpoint = kmalloc(i);

	int j;
	for (j = 0; j < i; j++) {
		mountpoint[j] = pathname[j+1];
	}
	mountpoint[i-1] = '\0';

	mounted_fs_t *aux = mount_list;
	while (aux != NULL) {
		if (strcmp(aux->name, mountpoint) == 0) {
			return aux->instance;
		}
		aux = aux->next;
	}
	return NULL;
}

int vfs_open(const char * pathname, open_file_descriptor *ofd, uint32_t modes) {
	int len;
	fs_instance_t *instance = get_instance_from_path(pathname, &len);
	if (instance->open != NULL) {
		return instance->open(instance, pathname + len, ofd, modes);
	}
	return -1;
}

void vfs_mount(const char *device __attribute__((unused)), const char *mountpoint, const char *type) {
	available_fs_t *aux = fs_list;
	while (aux != NULL) {
		if (strcmp(aux->fs->name, type) == 0) {
			mounted_fs_t *element = kmalloc(sizeof(mounted_fs_t));
			int len = strlen(mountpoint);
			element->name = kmalloc(len + 1);
			int i;
			for (i = 0; i <= len; i++)
				element->name[i] = mountpoint[i];
			element->instance = mount_FAT(); //XXX: device en argument.
			element->next = mount_list;
			mount_list = element;
		}
		aux = aux->next;
	}
}

int vfs_umount(const char *mountpoint) {
	mounted_fs_t *aux = mount_list;
	while (aux != NULL) {
		if (strcmp(aux->name, mountpoint) == 0) {
			if (aux->instance->fs->umount != NULL) {
				aux->instance->fs->umount(aux->instance);
			}
			return 0;
		}
		aux = aux->next;
	}
	return 1;
}


int vfs_mkdir(const char * pathname, mode_t mode) {
	int len;
	fs_instance_t *instance = get_instance_from_path(pathname, &len);
	if (instance && instance->mkdir != NULL) {
		instance->mkdir(instance, pathname + len, mode);
		return 0;
	}
	return -1;
}

int vfs_opendir(const char *pathname) {
	int len;
	fs_instance_t *instance = get_instance_from_path(pathname, &len);
	if (instance) {
		if (instance->opendir != NULL) {
			if (pathname[len] == '\0') {
				return instance->opendir(instance, "/");
			} else {
				return instance->opendir(instance, pathname + len);
			}
		}
	} else {
		if (len == 0) {
			return 0;
		}
	}
	return -1;
}

int vfs_readdir(const char * pathname, int iter, char * filename) {
	int len;
	fs_instance_t *instance = get_instance_from_path(pathname, &len);
	if (instance) {
		if (instance->readdir != NULL) {
			if (pathname[len] == '\0') {
				return instance->readdir(instance, "/", iter, filename);
			} else {
				return instance->readdir(instance, pathname + len, iter, filename);
			}
		}
	} else {
		if (len == 0) {
			mounted_fs_t *aux = mount_list;
			while (aux != NULL) {
				if (iter == 0) {
					strcpy(filename, aux->name);
					return 0;
				}
				aux = aux->next;
				iter--;
			}
		}
	}
	return -1;
}
