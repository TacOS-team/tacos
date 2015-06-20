/**
 * @file vfs.c
 *
 * @author TacOS developers 
 *
 * @section LICENSE
 *
 * Copyright (C) 2010-2015 TacOS developers.
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

#include <dcache.h>
#include <kdirent.h>
#include <kfcntl.h>
#include <kmalloc.h>
#include <vfs.h>
#include <kerrno.h>
#include <klibc/string.h>
#include <fd_types.h>
#include <klog.h>
#include <scheduler.h>

#define LOOKUP_PARENT 1 /**< S'arrête au niveau du parent. */
#define LOOKUP_NOFOLLOW 2 /**< Ne résoud pas le dernier lien. */

/**
 * Structure servant au lookup.
 */
struct nameidata {
	int flags; /**< Flags pour le lookup pour par exemple s'arréter au parent. */
	dentry_t *dentry; /**< Directory Entry. */
	mounted_fs_t *mnt; /**< Le FS utilisé. */
	char *last; /**< Ce qu'il reste du path à parcourir. */
};

static open_file_descriptor * dentry_open(dentry_t *dentry, mounted_fs_t *mnt, uint32_t flags);
extern struct _open_file_operations_t pipe_fops;

static dentry_t root_vfs;
static struct _open_file_operations_t vfs_fops = {.write = NULL, .read = NULL, .seek = generic_seek, .ioctl = NULL, .open = NULL, .close = NULL, .readdir = vfs_readdir};
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

static int open_namei(const char *pathname, struct nameidata *nb);

static struct _dentry_t* vfs_getroot (struct _fs_instance_t *instance __attribute__((unused))) {
	return &root_vfs;
}

void vfs_init() {
	root_vfs.d_name = "";
	root_vfs.d_pdentry = NULL;
	root_vfs.d_inode = kmalloc(sizeof(inode_t));
	root_vfs.d_inode->i_count = 0;
	root_vfs.d_inode->i_ino = 0;
	root_vfs.d_inode->i_size = 512;
	root_vfs.d_inode->i_blocks = 1;
	root_vfs.d_inode->i_nlink = 1;
	root_vfs.d_inode->i_mode = S_IFDIR | 00755;
	root_vfs.d_inode->i_fops = &vfs_fops;

	mvfs.instance = kmalloc(sizeof(fs_instance_t));
	memset(mvfs.instance, 0, sizeof(fs_instance_t));
	mvfs.instance->getroot = vfs_getroot;

// Non 100% fonctionnel.
	dcache_init();
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
	char *name = kmalloc(1);
	name[0] = '\0';
	
	if (*last) {
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
	int is_absolute_path = 0;
	dentry_t *dentry;
	// On va de dossier en dossier.
	while (*(nb->last)) {
		char *name = get_next_part_path(nb);

		if (is_absolute_path) {
			nb->mnt = get_mnt_from_path(name);
			if (nb->mnt) {
				if (nb->mnt->instance && nb->mnt->instance->getroot) {
					nb->dentry = nb->mnt->instance->getroot(nb->mnt->instance);
				} else {
					kerr("instance ou getroot null");
					return -1;
				}
			} else if (name[0] == '\0') {
				nb->mnt = &mvfs;
				nb->dentry = &root_vfs;
				nb->dentry->d_inode->i_count++;
				return 0;
			}

			is_absolute_path = 0;
			kfree(name);
			continue;
		}

		if (nb->dentry->d_pdentry == NULL && strcmp(name, "..") == 0) {
			is_absolute_path = 1;
			kfree(name);
			continue;
		}


		if (*(nb->last) == '\0') { // Si c'est le dernier élément
			if (nb->flags & LOOKUP_PARENT) {
				nb->last = name;
				break;
			}
		}

		dentry = dcache_get(nb->mnt->instance, nb->dentry, name);
		if (!dentry) {
			dentry = nb->mnt->instance->lookup(nb->mnt->instance, nb->dentry, name);
			if (dentry) {
				dcache_set(nb->mnt->instance, nb->dentry, name, dentry);
			}
		}
		if (dentry) {
			if (S_ISLNK(dentry->d_inode->i_mode) && !(*(nb->last) == '\0' && (nb->flags & LOOKUP_NOFOLLOW))) {
				open_file_descriptor *ofd = dentry_open(dentry, nb->mnt, O_RDONLY);
				if (ofd && ofd->f_ops->read) {
					char link[255];
					int ret = ofd->f_ops->read(ofd, link, sizeof(link));
					if (ret >= 0) {
						link[ret] = '\0';

						char *newpath = kmalloc(sizeof(char) * (ret + strlen(nb->last)) + 1);
						strcpy(newpath, link);
						strcpy(newpath + ret, nb->last);
						kfree(nb->last);
						nb->last = newpath;

						if (nb->last[0] == '/') {
							is_absolute_path = 1;
						}
					} else {
						kfree(name);
						kfree(ofd);
						return -1;
					}
				}
				kfree(ofd);
			} else {
				nb->dentry = dentry;
			}
		} else {
			kfree(name);
			return -1;
		}

		kfree(name);
	}
	return 0;
}

static int open_namei(const char *pathname, struct nameidata *nb) {
	char *path;
	size_t len = strlen(pathname);
	if (pathname[0] != '/') {
		process_t *process = get_current_process();
		size_t len2 = strlen(process->cwd);
		path = kmalloc(len + len2 + 1);
		strcpy(path, process->cwd);
		strcpy(path + len2, pathname);
		len += len2;
	} else {
		path = kmalloc(len + 1);
		strcpy(path, pathname);
	}

	while (path[len - 1] == '/') {
		path[--len] = '\0';
	}
	nb->last = path;
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
	} else if (len == 0) {
		nb->mnt = &mvfs;
		nb->dentry = &root_vfs;
		nb->dentry->d_inode->i_count++;
		return 0;
	}
	return -2;
}

int vfs_chdir(const char* path) {
	process_t* process = get_current_process();
	int len = 0;
	int len2 = strlen(path);
	
	char *pathname;
	if (path[0] != '/') {
		len = strlen(process->cwd);
	}
	pathname = kmalloc(len + len2 + 2);

	if (len) {
		strcpy(pathname, process->cwd);
	}
	strcpy(pathname + len, path);

	int lenp = 1;
	struct nameidata nb;
	nb.last = strdup(pathname);
	pathname[lenp] = '\0';

	while (*(nb.last)) {
		char* name = get_next_part_path(&nb);

		if (name[0] == '.') {
			if (name[1] == '.' && name[2] == '\0') {
				pathname[lenp - 1] = '\0';
				char *r = strrchr(pathname, '/');
				if (r != NULL) {
					lenp = r - pathname + 1;
				}
				strcpy(pathname + lenp - 1, "/");
				kfree(name);
				continue;
			} else if (name[1] == '\0') {
				kfree(name);
				continue;
			}
		}

		strcpy(pathname + lenp, name);
		int l = strlen(name);
		if (l > 0) {
			lenp += l;
			pathname[lenp++] = '/';
		}
		pathname[lenp] = '\0';
		kfree(name);
	}

	nb.flags = 0;
	int ret = open_namei(pathname, &nb);
	if (ret == 0) {
		if (!S_ISDIR(nb.dentry->d_inode->i_mode)) {
			return -ENOTDIR;
		}
		char *oldpath = process->cwd;
		process->cwd = pathname;
		kfree(oldpath);
	}

	return ret;
}


static open_file_descriptor * dentry_open(dentry_t *dentry, mounted_fs_t *mnt, uint32_t flags) {
	dentry->d_inode->i_count++;

	open_file_descriptor *ofd = kmalloc(sizeof(open_file_descriptor));
	ofd->pathname = "";
	ofd->flags = flags;
	ofd->inode = dentry->d_inode;
	ofd->dentry = dentry;
	ofd->mnt = mnt;
	ofd->current_octet = 0;
	ofd->i_fs_specific = dentry->d_inode->i_fs_specific;
	ofd->extra_data = NULL;
	ofd->select_sem = 0;

	if (S_ISFIFO(dentry->d_inode->i_mode)) {
		// klog("ouverture d'un pipe !");
		// klog("i count : %d", dentry->d_inode->i_count);
		ofd->f_ops = &pipe_fops;
		ofd->fs_instance = NULL;
	} else {
		ofd->f_ops = dentry->d_inode->i_fops;
		ofd->fs_instance = mnt->instance;
	}

	return ofd;
}

open_file_descriptor * vfs_open(const char * pathname, uint32_t flags) {
	struct nameidata nb;
	open_file_descriptor *ret = NULL;
	nb.flags = LOOKUP_PARENT;

	if (flags & O_NOFOLLOW) {
		nb.flags |= LOOKUP_NOFOLLOW;
	}

	if (open_namei(pathname, &nb) == 0) {
		if (!(flags & O_CREAT)) {
			nb.flags &= ~LOOKUP_PARENT;
			if (lookup(&nb) != 0) {
				return NULL;
			}
		} else {
			struct nameidata nb_last;
			memcpy(&nb_last, &nb, sizeof(struct nameidata));
			nb_last.flags &= ~LOOKUP_PARENT;
			if (lookup(&nb_last) != 0) {
				dentry_t *new_entry = kmalloc(sizeof(dentry_t));
				new_entry->d_name = nb.last;
				// klog("vfs_open create d_name : %s", nb.last);
				new_entry->d_pdentry = nb.dentry;
				if (nb.mnt->instance->mknod(nb.dentry->d_inode, new_entry, 0, 0) < 0) {
					return NULL;
				}
				ret = dentry_open(new_entry, nb.mnt, flags);
				goto ok;
			} else {
				memcpy(&nb, &nb_last, sizeof(struct nameidata));
			}
		}
		ret = dentry_open(nb.dentry, nb.mnt, flags);
	}
ok:
	if (ret != NULL) {
		if (flags & O_TRUNC && nb.mnt->instance->setattr) {
			file_attributes_t attr;
			attr.mask = ATTR_SIZE;
			attr.ia_size = 0;
			nb.mnt->instance->setattr(nb.dentry->d_inode, &attr);
		}
		ret->pathname = strdup(pathname);

		if (ret->f_ops->open) {
			ret->f_ops->open(ret);
		}
	}

	return ret;
}

int vfs_close(open_file_descriptor *ofd) {
	if (ofd == NULL) {
		return -1;
	}
/*	klog("vfs close %s", ofd->pathname);

	klog("dentry: %d", ofd->dentry);
	klog("d_name: %s", ofd->dentry->d_name);
	klog("d_inode: %d", ofd->dentry->d_inode);
	klog("d_pdentry: %d", ofd->dentry->d_pdentry);
*/
	ofd->dentry->d_inode->i_count--;
	if (ofd->dentry->d_inode->i_count == 0 && ofd->dentry->d_pdentry) {
			dcache_remove(ofd->fs_instance, ofd->dentry->d_pdentry, ofd->dentry->d_name);
	} else {
//			klog("i_count ok : %s %d", ofd->dentry->d_name, ofd->dentry->d_inode->i_count);
	}
	kfree(ofd->pathname);
	kfree(ofd);
	return 0;
}

void vfs_mount(const char *device, const char *mountpoint, const char *type) {
	available_fs_t *aux = fs_list;
	open_file_descriptor* ofd = NULL;
	while (aux != NULL) {
		if (strcmp(aux->fs->name, type) == 0) {
			mounted_fs_t *element = kmalloc(sizeof(mounted_fs_t));
			element->name = strdup(mountpoint);
				
			if (device != NULL) {
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
// st_rdev;	 /**< Type périphérique	*/
	buf->st_size = inode->i_size;
	buf->st_blocks = inode->i_blocks;
	buf->st_blksize = 512;
	buf->st_atime = inode->i_atime;
	buf->st_mtime = inode->i_mtime;
	buf->st_ctime = inode->i_ctime;
}

int vfs_stat(const char *pathname, struct stat *buf, int follow_link) {
	struct nameidata nd;
	if (follow_link == 0) {
		nd.flags = LOOKUP_NOFOLLOW;
	} else {
		nd.flags = 0;
	}
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
			dentry_t *dentry = nb.dentry;
			const char *last = nb.last;
			if (lookup(&nb) < 0) {
				return -ENOENT;
			}
			//FIXME
			//TODO: indiquer ce qu'il faut fixer...
			dcache_remove(nb.mnt->instance, dentry, last);
			nb.mnt->instance->unlink(pinode, nb.dentry);
		} else {
			klog("Pas de unlink pour ce fs.");
			return -EPERM;
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
			dentry_t *dentry = nb.dentry;
			const char *last = nb.last;
			if (lookup(&nb) < 0) {
				return -ENOENT;
			}
			//FIXME
			dcache_remove(nb.mnt->instance, dentry, last);
			nb.mnt->instance->rmdir(pinode, nb.dentry);
		} else {
			klog("Pas de rmdir pour ce fs.");
			return -EPERM;
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
			inode_t *pinode = nb.dentry->d_inode;
			dentry_t new_entry;
			new_entry.d_name = nb.last;

			// Check existe pas déjà.
			nb.flags &= ~LOOKUP_PARENT;
			if (lookup(&nb) == 0) {
				return -EEXIST;
			}

			nb.mnt->instance->mknod(pinode, &new_entry, mode, dev);
		} else {
			klog("Pas de mknod pour ce fs.");
			return -EPERM;
		}
		return 0;
	} else {
		return -ENOENT;
	}
}

int vfs_symlink(const char * target, const char * linkpath) {
	struct nameidata nb;
	nb.flags = LOOKUP_PARENT;
	if (open_namei(linkpath, &nb) == 0) {
		if (nb.mnt->instance->symlink) {
			inode_t *pinode = nb.dentry->d_inode;
			dentry_t new_entry;
			new_entry.d_name = nb.last;

			// Check existe pas déjà.
			nb.flags &= ~LOOKUP_PARENT;
			if (lookup(&nb) == 0) {
				return -EEXIST;
			}

			nb.mnt->instance->symlink(pinode, &new_entry, target);
		} else {
			klog("Pas de symlink pour ce fs.");
			return -EPERM;
		}
		return 0;
	} else {
		return -ENOENT;
	}
}

int vfs_mkdir(const char * pathname, mode_t mode) {
	struct nameidata nb;
	nb.flags = LOOKUP_PARENT;
	if (open_namei(pathname, &nb) == 0) {
		if (nb.mnt->instance->mkdir) {
			inode_t *pinode = nb.dentry->d_inode;
			dentry_t new_entry;
			new_entry.d_name = nb.last;

			// Check existe pas déjà.
			nb.flags &= ~LOOKUP_PARENT;
			if (lookup(&nb) == 0) {
				return -EEXIST;
			}

			nb.mnt->instance->mkdir(pinode, &new_entry, mode);
		} else {
			klog("Pas de mkdir pour ce fs.");
			return -EPERM;
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
			if (lookup(&nb)) {
				// Ancien n'existe pas...
				return -ENOENT;
			}
			dentry_t *old_dentry = nb.dentry;

			nb.flags = LOOKUP_PARENT;
			if (open_namei(newpath, &nb) == 0) {
				dentry_t new_entry;
				new_entry.d_name = nb.last;
				// TODO: check destination est un fichier ou un repertoire vide ou n'existe pas.

				return nb.mnt->instance->rename(old_dir, old_dentry, nb.dentry->d_inode, &new_entry);
			} else {
				return -ENOENT;
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

ssize_t vfs_readlink(const char *path, char *buf, size_t bufsize) {
	open_file_descriptor *ofd = vfs_open(path, O_RDONLY | O_NOFOLLOW);
	if (ofd && ofd->f_ops->read) {
		return ofd->f_ops->read(ofd, buf, bufsize);
	}
	return -1;
}

int generic_seek(open_file_descriptor * ofd, long offset, int whence) {
	switch (whence) {
	case SEEK_SET:
		if ((uint32_t)offset > ofd->dentry->d_inode->i_size) {
			return -1;
		}
		ofd->current_octet = offset;
		break;
	case SEEK_CUR:
		if (ofd->current_octet + (uint32_t)offset > ofd->dentry->d_inode->i_size) {
			return -1;
		}
		ofd->current_octet += offset;
		break;
	case SEEK_END:
		if ((uint32_t)offset > ofd->dentry->d_inode->i_size) {
			return -1;
		}
		ofd->current_octet = ofd->dentry->d_inode->i_size - offset;
		break;
	}

	return 0;
}


