/**
 * @file vfs.c
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
 * @brief Virtual File System. Gestion des points de montage.
 */

#include <fs/devfs.h>
#include <fs/fat.h>
#include <kdirent.h>
#include <kfcntl.h>
#include <kmalloc.h>
#include <vfs.h>
#include <kerrno.h>
#include <klibc/string.h>
#include <fd_types.h>
#include <klog.h>

#define LOOKUP_PARENT 1 /**< S'arrête au niveau du parent. */

static dentry_t root_vfs;
static struct _open_file_operations_t vfs_fops = {.write = NULL, .read = NULL, .seek = NULL, .ioctl = NULL, .open = NULL, .close = NULL, .readdir = vfs_readdir};
static mounted_fs_t mvfs;

/**
 * Cellule d'une liste de fs disponibles.
 */
typedef struct _available_fs_t {
	file_system_t *fs; /**< Informations sur le FS. */
	struct _available_fs_t *next; /**< FS suivant. */
} available_fs_t;

static available_fs_t *fs_list;

static mounted_fs_t *mount_list;

static struct _dentry_t* vfs_getroot (struct _fs_instance_t *instance __attribute__((unused))) {
	return &root_vfs;
}

void vfs_init() {
	root_vfs.d_name = "";
	root_vfs.d_inode = kmalloc(sizeof(inode_t));
	root_vfs.d_inode->i_ino = 0;
	root_vfs.d_inode->i_size = 512;
	root_vfs.d_inode->i_blocks = 1;
	root_vfs.d_inode->i_nlink = 1;
	root_vfs.d_inode->i_mode = S_IFDIR | 00755;
	root_vfs.d_inode->i_fops = &vfs_fops;

	mvfs.instance = kmalloc(sizeof(fs_instance_t));
	mvfs.instance->device = NULL;
	mvfs.instance->getroot = vfs_getroot;
	mvfs.instance->lookup = NULL;
}

void vfs_register_fs(file_system_t *fs) {
	available_fs_t *element = kmalloc(sizeof(available_fs_t));
	element->fs = fs;
	element->next = fs_list;
	fs_list = element;
}

static mounted_fs_t* get_mnt_from_path(const char * name) {
	mounted_fs_t *aux = mount_list;
	while (aux != NULL) {
		if (strcmp(aux->name, name) == 0) {
			return aux;
		}
		aux = aux->next;
	}
	return NULL;
}

static char * get_next_part_path(struct nameidata *nb) {
	const char *last = nb->last;
	char *name = NULL;
	
	if (last) {
		while (*last == '/') last++;
		char *p = strchrnul(last, '/');
		name = kmalloc(p - last + 1);
		strncpy(name, last, p - last);
		name[p - last] = '\0';
		nb->last = p;
	}

	return name;
}

static int lookup(struct nameidata *nb) {
	dentry_t *dentry;
	// On va de dossier en dossier.
	while (*(nb->last)) {
		const char *name = get_next_part_path(nb);
		if (name[0] == '.') {
			if (name[1] == '\0') {
				continue;
			} else if (name[1] == '.' && name[2] == '\0') {
				// TODO: remonter d'un niveau.
			}
		}

		if (*(nb->last) == '\0') { // Si c'est le dernier élément
			if (nb->flags & LOOKUP_PARENT) {
				nb->last = name;
				break;
			}
		}

		dentry = nb->mnt->instance->lookup(nb->mnt->instance, nb->dentry, name);
		if (dentry) {
			nb->dentry = dentry;
		} else {
			return -1;
		}
	}
	return 0;
}

static int open_namei(const char *pathname, struct nameidata *nb) {
	nb->last = pathname;
	nb->mnt = get_mnt_from_path(get_next_part_path(nb));
	if (nb->mnt) {
		if (nb->mnt->instance && nb->mnt->instance->getroot) {
			nb->dentry = nb->mnt->instance->getroot(nb->mnt->instance);
		} else {
			kerr("instance ou getroot null");
			return -1;
		}

		// Si c'est le dernier élément
		if (*(nb->last) == '\0') {
			if (nb->flags & LOOKUP_PARENT) {
				return 0;
			}
		}

		return lookup(nb);
	} else if (strlen(pathname) <= 1) {
		nb->mnt = &mvfs;
		nb->dentry = &root_vfs;
		return 0;
	}
	return -2;
}

static open_file_descriptor * dentry_open(dentry_t *dentry, mounted_fs_t *mnt, uint32_t flags) {
	open_file_descriptor *ofd = kmalloc(sizeof(open_file_descriptor));
	ofd->flags = flags;
	ofd->dentry = dentry;
	ofd->mnt = mnt;
	ofd->file_size = dentry->d_inode->i_size;
	ofd->current_octet = 0;
	ofd->f_ops = dentry->d_inode->i_fops;
	ofd->fs_instance = mnt->instance;
	ofd->extra_data = dentry->d_inode->i_fs_specific;

	if (ofd->f_ops->open) {
		ofd->f_ops->open(ofd);
	}

	return ofd;
}

open_file_descriptor * vfs_open(const char * pathname, uint32_t flags) {
	struct nameidata nb;
	open_file_descriptor *ret = NULL;
	nb.flags = LOOKUP_PARENT;
	if (open_namei(pathname, &nb) == 0) {
		if (!(flags & O_CREAT)) {
			nb.flags &= ~LOOKUP_PARENT;
			if (lookup(&nb) != 0) {
				goto end;
			}
		} else {
			struct nameidata nb_last;
			memcpy(&nb_last, &nb, sizeof(struct nameidata));
			nb_last.flags &= ~LOOKUP_PARENT;
			if (lookup(&nb_last) != 0) {
				dentry_t new_entry;
				new_entry.d_name = nb.last;
				nb.mnt->instance->mknod(nb.dentry->d_inode, &new_entry, 0, 0); //XXX
				ret = dentry_open(&new_entry, nb.mnt, flags);
				goto ok;
			} else {
				memcpy(&nb, &nb_last, sizeof(struct nameidata));
			}
		}
		ret = dentry_open(nb.dentry, nb.mnt, flags);
		goto ok;
	}

end:
	return ret;
ok:
	if (flags & O_TRUNC && nb.mnt->instance->setattr) {
			file_attributes_t attr;
			attr.mask = ATTR_SIZE;
			attr.ia_size = 0;
			nb.mnt->instance->setattr(nb.dentry->d_inode, &attr);
	}
	goto end;
}

int vfs_close(open_file_descriptor *ofd) {
	if (ofd == NULL) {
		return -1;
	}
	kfree(ofd);
	return 0;
}

void vfs_mount(const char *device, const char *mountpoint, const char *type) {
	available_fs_t *aux = fs_list;
	open_file_descriptor* ofd = NULL;
	while (aux != NULL) {
		if (strcmp(aux->fs->name, type) == 0) {
			mounted_fs_t *element = kmalloc(sizeof(mounted_fs_t));
			int len = strlen(mountpoint);
			element->name = kmalloc(len + 1);
			int i;
			for (i = 0; i <= len; i++)
				element->name[i] = mountpoint[i];
				
			if(device != NULL) {
				/* Open the mounted device */
				ofd = vfs_open(device, O_RDWR);
			}
			element->instance = aux->fs->mount(ofd);
			element->instance->device = ofd;
			element->next = mount_list;
			mount_list = element;
			root_vfs.d_inode->i_nlink++;
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
				/* Close the device ofd. */
				if (aux->instance->device != NULL && aux->instance->device->f_ops->close) {
					aux->instance->device->f_ops->close(aux->instance->device);
				}
			}
			root_vfs.d_inode->i_nlink--;
			return 0;
		}
		aux = aux->next;
	}
	return 1;
}

static void fill_stat_from_inode(inode_t *inode, struct stat *buf) {
	// TODO :
//	buf->st_dev = inode->instance->
	buf->st_ino = inode->i_ino;
	buf->st_mode = inode->i_mode;
	buf->st_nlink = inode->i_nlink;
	buf->st_uid = inode->i_uid;
	buf->st_gid = inode->i_gid;
// st_rdev;     /**< Type périphérique               */
	buf->st_size = inode->i_size;
	buf->st_blocks = inode->i_blocks;
	buf->st_blksize = 512;
	buf->st_atime = inode->i_atime;
	buf->st_mtime = inode->i_mtime;
	buf->st_ctime = inode->i_ctime;
}

int vfs_stat(const char *pathname, struct stat *buf) {
	struct nameidata nd;
	nd.flags = 0;
	if (open_namei(pathname, &nd) == 0) {
		// TODO: appeler fonction stat du FS.
		fill_stat_from_inode(nd.dentry->d_inode, buf);
		return 0;
	} else {
		return -ENOENT;
	}
}

int vfs_unlink(const char *pathname) {
	struct nameidata nb;
	nb.flags = LOOKUP_PARENT;
	if (open_namei(pathname, &nb) == 0) {
		if (nb.mnt->instance->unlink) {
			inode_t *pinode = nb.dentry->d_inode;
			nb.flags &= ~LOOKUP_PARENT;
			lookup(&nb);
			nb.mnt->instance->unlink(pinode, nb.dentry);
		} else {
			klog("Pas de unlink pour ce fs.");
		}
		return 0;
	} else {
		return -ENOENT;
	}
}

int vfs_rmdir(const char *pathname) {
	struct nameidata nb;
	nb.flags = LOOKUP_PARENT;
	if (open_namei(pathname, &nb) == 0) {
		if (nb.mnt->instance->rmdir) {
			inode_t *pinode = nb.dentry->d_inode;
			nb.flags &= ~LOOKUP_PARENT;
			lookup(&nb);
			nb.mnt->instance->rmdir(pinode, nb.dentry);
		} else {
			klog("Pas de rmdir pour ce fs.");
		}
		return 0;
	} else {
		return -ENOENT;
	}
}

int vfs_mknod(const char * pathname, mode_t mode, dev_t dev) {
	struct nameidata nb;
	nb.flags = LOOKUP_PARENT;
	if (open_namei(pathname, &nb) == 0) {
		if (nb.mnt->instance->mknod) {
			dentry_t new_entry;
			new_entry.d_name = nb.last;
			nb.mnt->instance->mknod(nb.dentry->d_inode, &new_entry, mode, dev);
		} else {
			klog("Pas de mknod pour ce fs.");
		}
		return 0;
	} else {
		return -ENOENT;
	}
}

int vfs_mkdir(const char * pathname, mode_t mode) {
	struct nameidata nd1;
	nd1.flags = 0;
	if (open_namei(pathname, &nd1) == 0) {
		return -EEXIST;
	}

	struct nameidata nb;
	nb.flags = LOOKUP_PARENT;
	if (open_namei(pathname, &nb) == 0) {
		if (nb.mnt->instance->mkdir) {
			dentry_t new_entry;
			new_entry.d_name = nb.last;
			nb.mnt->instance->mkdir(nb.dentry->d_inode, &new_entry, mode);
		} else {
			klog("Pas de mkdir pour ce fs.");
		}
		return 0;
	} else {
		return -ENOENT;
	}
}

int vfs_chmod(const char *pathname, mode_t mode) {
	struct nameidata nb;
	if (open_namei(pathname, &nb) == 0) {
		if (nb.mnt->instance->setattr) {
			file_attributes_t attr;
			attr.mask = ATTR_MODE;
			attr.stbuf.st_mode = mode;
			nb.mnt->instance->setattr(nb.dentry->d_inode, &attr);
		}
	}
	return 0;
}

int vfs_chown(const char *pathname, uid_t owner, gid_t group) {
	struct nameidata nb;
	if (open_namei(pathname, &nb) == 0) {
		if (nb.mnt->instance->setattr) {
			file_attributes_t attr;
			attr.mask = ((int)owner >= 0 ? ATTR_UID : 0) | ((int)group >= 0 ? ATTR_GID : 0);
			attr.stbuf.st_uid = owner;
			attr.stbuf.st_gid = group;
			nb.mnt->instance->setattr(nb.dentry->d_inode, &attr);
		}
	}
	return 0;
}

int vfs_utimes(const char *pathname, const struct timeval tv[2]) {
	struct nameidata nb;
	if (open_namei(pathname, &nb) == 0) {
		if (nb.mnt->instance->setattr) {
			file_attributes_t attr;
			attr.mask = ATTR_ATIME | ATTR_MTIME;
			attr.stbuf.st_atime = tv[0].tv_sec;
			attr.stbuf.st_mtime = tv[1].tv_sec;
			nb.mnt->instance->setattr(nb.dentry->d_inode, &attr);
		}
	}
	return 0;
}

int vfs_rename(const char *oldpath, const char *newpath) {
//TODO: rename entre 2 disques différents
	struct nameidata nb;
	nb.flags = LOOKUP_PARENT;
	if (open_namei(oldpath, &nb) == 0) {
		if (nb.mnt->instance->rename) {
			inode_t *old_dir = nb.dentry->d_inode;
			nb.flags &= ~LOOKUP_PARENT;
			lookup(&nb);
			dentry_t *old_dentry = nb.dentry;

			nb.flags = LOOKUP_PARENT;
			if (open_namei(newpath, &nb) == 0) {
				dentry_t new_entry;
				new_entry.d_name = nb.last;
	
				nb.mnt->instance->rename(old_dir, old_dentry, nb.dentry->d_inode, &new_entry);
			}
		} else {
			klog("Pas de rename pour ce fs.");
		}
		return 0;
	} else {
		return -ENOENT;
	}
}

// ------

int vfs_readdir(open_file_descriptor * ofd, char * entries, size_t size) {
	size_t count = 0;
	size_t c = 0;
	mounted_fs_t *aux = mount_list;

	while (c < ofd->current_octet && aux) {
		aux = aux->next;
		c++;
	}

	while (aux != NULL && count < size) {
		struct dirent *d = (struct dirent *)(entries + count);
		d->d_ino = 1;
		d->d_reclen = sizeof(d->d_ino) + sizeof(d->d_reclen) + sizeof(d->d_type) + strlen(aux->name) + 1;
		//d.d_type = dir_entry->attributes;
		strcpy(d->d_name, aux->name);
		count += d->d_reclen;
		aux = aux->next;
		c++;
	}

	ofd->current_octet = c;

	return count;
}
