/**
 * @file ext2_functions.c
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

#include "ext2_internal.c"

#include <klog.h>
#include <types.h>
#include <kfcntl.h>
#include <kerrno.h>

#include <fs/ext2.h>

// XXX: Fonctions à intégrer.
#ifdef FALSE

static int ext2_utimens(fs_instance_t *instance, const char *path, const struct timespec tv[2]) {
	int inode = getinode_from_path(path);
	if (inode >= 0) {
		struct stat s;
		getattr_inode(inode, &s);
		s.st_atime = tv[0].tv_sec;
		s.st_mtime = tv[1].tv_sec;
		setattr_inode(inode, &s);
		return 0;
	} else {
		return inode;
	}
}

static int ext2_rename(fs_instance_t *instance, const char *orig, const char *dest) {
	ext2_fs_instance_t* inst = (ext2_fs_instance_t*)instance;
	int inode = getinode_from_path(inst, orig);
	if (inode > 0) {
		// Get parent dir.
		char filename[256];
		char * dir = kmalloc(strlen(orig));
		split_dir_filename(orig, dir, filename);
		int dir_inode = getinode_from_path(inst, dir);

		// Remove inode from parent dir.
		remove_dir_entry(inst, dir_inode, filename);

		// Add inode to dest.
		dir = kmalloc(strlen(dest));
		split_dir_filename(dest, dir, filename);
		int dest_inode = getinode_from_path(inst, dir);
		add_dir_entry(inst, dest_inode, filename, EXT2_FT_REG_FILE, inode); //XXX
		return 0;
	} else {
		return inode;
	}
}

#endif

int ext2_rmdir(fs_instance_t *instance, const char * path) {
	int inode = getinode_from_path((ext2_fs_instance_t*)instance, path);
	if (inode > 0) {
		char filename[256];
		char * dir = kmalloc(strlen(path));
		split_dir_filename(path, dir, filename);
		inode = getinode_from_path((ext2_fs_instance_t*)instance, dir);
		// TODO: Check is dir.
		// TODO: Call rmdir_inode!
		remove_dir_entry((ext2_fs_instance_t*)instance, inode, filename);
		// Free !
		return 0;
	} else {
		return inode;
	}
}

// XXX! Virer ce buffer.
static void load_buffer(open_file_descriptor *ofd) {
	ext2_fs_instance_t *instance = (ext2_fs_instance_t*) ofd->fs_instance;
	size_t size_buffer = sizeof(ofd->buffer) < (size_t) (1024 << instance->superblock.s_log_block_size) ? 
			sizeof(ofd->buffer) : (size_t) (1024 << instance->superblock.s_log_block_size);
	off_t offset = ofd->current_octet;

	struct blk_t *blocks = ((ext2_extra_data*)ofd->extra_data)->blocks;
	if (blocks == NULL) {
		blocks = addr_inode_data(instance, ((ext2_extra_data*)ofd->extra_data)->inode);
	}
	while (offset >= (unsigned int)(1024 << instance->superblock.s_log_block_size)) {
		if (blocks && blocks->next) {
			blocks = blocks->next;
		} else return;
		offset -= 1024 << instance->superblock.s_log_block_size;
	}

	size_t count = 0;
	while (count < size_buffer && blocks != NULL) {
		int addr = blocks->addr + offset;
		size_t size2 = (1024 << instance->superblock.s_log_block_size) - offset;
		offset = 0;
		if (size2 > size_buffer - count) {
			size2 = size_buffer - count;
		}

		instance->read_data(instance->super.device, (char*)(ofd->buffer) + count, size2, addr);
		count += size2;

		blocks = blocks->next;
	}
	ofd->current_octet_buf = 0;
}


int ext2_chmod(inode_t *inode, mode_t mode) {
	struct stat s;
	ext2_fs_instance_t *inst = (ext2_fs_instance_t*)inode->i_instance;
	getattr_inode(inst, inode->i_ino, &s);
	s.st_mode = mode;
	setattr_inode(inst, inode->i_ino, &s);
	return 0;
}

int ext2_chown(inode_t *inode, uid_t uid, gid_t gid) {
	struct stat s;
	getattr_inode((ext2_fs_instance_t*)inode->i_instance, inode->i_ino, &s);
	s.st_uid = uid;
	s.st_gid = gid;
	setattr_inode((ext2_fs_instance_t*)inode->i_instance, inode->i_ino, &s);
	return 0;
}

int ext2_unlink(inode_t *dir, dentry_t *dentry) {
	remove_dir_entry((ext2_fs_instance_t*)dir->i_instance, dir->i_ino, dentry->d_name);
	// TODO: nlink--
	return 0;
}

int ext2_mkdir(inode_t *dir, dentry_t *dentry, mode_t mode) {
	return ext2_mknod(dir, dentry, mode | EXT2_S_IFDIR, 0);
}

/*
int ext2_stat(dentry_t *dentry, struct stat *stbuf) {
	getattr_inode((ext2_fs_instance_t*)dentry->d_inode->i_instance, dentry->d_inode, stbuf);
	return 0;
}
*/

int ext2_readdir(open_file_descriptor * ofd, char * entries, size_t size) {
	size_t count = 0;
	ext2_fs_instance_t *instance = (ext2_fs_instance_t*) ofd->fs_instance;
	int inode = ((ext2_extra_data*)ofd->extra_data)->inode;
	if (inode >= 0) {
		size_t c = 0;
		struct directories_t *dirs = readdir_inode(instance, inode);
		struct directories_t *aux = dirs;

		while (c < ofd->current_octet && aux != NULL) {
			aux = aux->next;
			c++;
		}

		while (aux != NULL && count < size) {
			struct dirent *d = (struct dirent *)(entries + count);
			d->d_ino = aux->dir->inode;
			strncpy(d->d_name, aux->dir->name, aux->dir->name_len);
			d->d_name[aux->dir->name_len] = '\0';
			d->d_reclen = sizeof(d->d_ino) + sizeof(d->d_reclen) + sizeof(d->d_type) + strlen(d->d_name) + 1;
			d->d_type = aux->dir->file_type;
			count += d->d_reclen;
			c++;
			aux = aux->next;
		}

		ofd->current_octet = c;
	}
	return count;
}

int ext2_seek(open_file_descriptor * ofd, long offset, int whence) {
	switch (whence) {
	case SEEK_SET:
		if ((uint32_t)offset > ofd->file_size) {
			return -1;
		}
		ofd->current_octet = offset;
		load_buffer(ofd);
		break;
	case SEEK_CUR:
		if (ofd->current_octet + (uint32_t)offset > ofd->file_size) {
			return -1;
		}
		ofd->current_octet += offset;
		load_buffer(ofd);
		break;
	case SEEK_END:
		if ((uint32_t)offset > ofd->file_size) {
			return -1;
		}
		ofd->current_octet = ofd->file_size - offset;
		load_buffer(ofd);
		break;
	}

	return 0;
}

size_t ext2_write (open_file_descriptor * ofd, const void *buf, size_t size) {
	if ((ofd->flags & O_ACCMODE) == O_RDONLY) {
		return 0;
	}

	int inode = ((ext2_extra_data*)ofd->extra_data)->inode;
	if (inode >= 0) {
		ext2_fs_instance_t *instance = (ext2_fs_instance_t*) ofd->fs_instance;
		
		struct ext2_inode einode;
		if (read_inode(instance, inode, &einode) == 0) {
			unsigned int offset;
			if (ofd->flags & O_APPEND) {
				offset = einode.i_size;
			} else {
			 	offset = ofd->current_octet;
			}

			int count = 0;
			struct blk_t *last = NULL;
			struct blk_t *blocks = ((ext2_extra_data*)ofd->extra_data)->blocks;
			if (blocks == NULL) {
				blocks = addr_inode_data(instance, inode);
			}
			struct blk_t *aux = blocks;

			int off = offset;
			while (aux != NULL && off >= (1024 << instance->superblock.s_log_block_size)) {
				last = aux;
				aux = aux->next;
				off -= 1024 << instance->superblock.s_log_block_size;
			}

			while (size > 0) {
				if (aux == NULL) {
					struct blk_t *element = kmalloc(sizeof(struct blk_t));
					element->addr = alloc_block(instance) * (1024 << instance->superblock.s_log_block_size);
					element->next = NULL;
					if (last == NULL) {
						blocks = element;
					} else {
						last->next = element;
					}
					aux = element;
				}
				int addr = aux->addr + off;
				size_t size2 = (1024 << instance->superblock.s_log_block_size) - off;
				off = 0;
				if (size2 > size) {
					size2 = size;
				}

				instance->write_data(instance->super.device, ((char*)buf) + count, size2, addr);
				size -= size2;
				count += size2;

				last = aux;
				aux = aux->next;
			}

			einode.i_size = max(einode.i_size, offset + count);
	//		struct timeval tv;
	//		gettimeofday(&tv, NULL);
	//		einode.i_mtime = tv.tv_sec;
			update_blocks(instance, &einode, blocks);
			write_inode(instance, inode, &einode);
			return count;		
		} else {
			return -ENOENT;
		}
		return size;
	} else {
		return inode;
	}
}

size_t ext2_read(open_file_descriptor * ofd, void * buf, size_t size) {
	if ((ofd->flags & O_ACCMODE) == O_WRONLY) {
		return 0;
	}

	int inode = ((ext2_extra_data*)ofd->extra_data)->inode;
	if (inode >= 0) {
		ext2_fs_instance_t *instance = (ext2_fs_instance_t*) ofd->fs_instance;
		size_t size_buffer = sizeof(ofd->buffer) < (size_t) (1024 << instance->superblock.s_log_block_size) ? 
			sizeof(ofd->buffer) : (size_t) (1024 << instance->superblock.s_log_block_size);
		size_t count = 0;
		int j = 0;
	
		while (size--) {
			if (ofd->current_octet == ofd->file_size) {
				break;
			} else {
				char c = ofd->buffer[ofd->current_octet_buf];
				count++;
				ofd->current_octet_buf++;
				ofd->current_octet++;
				((char*) buf)[j++] = c;
				if (ofd->current_octet_buf >= size_buffer) {
					load_buffer(ofd);
				}
			}
		}

		// Commenté pour des raisons évidentes de perf.
		//	struct timeval tv;
		//	gettimeofday(&tv, NULL);
		//	einode.i_atime = tv.tv_sec;
		//	write_inode(inode, &einode);
 		
		return count;
	} else {
		return inode;
	}
}


int ext2_mknod(inode_t *dir, dentry_t *dentry, mode_t mode, dev_t dev) {
	int ino =  mknod_inode((ext2_fs_instance_t*)dir->i_instance, dir->i_ino, dentry->d_name, mode, dev);
	if (ino == 0) {
		return -ENOTDIR; //XXX
	}
	struct ext2_inode einode;
	read_inode((ext2_fs_instance_t*)dir->i_instance, ino, &einode);

	inode_t *inode = ext2inode_2_inode(dir->i_instance, ino, &einode);
	dentry->d_inode = inode;

	return 0;
}

int ext2_truncate(inode_t *inode, off_t off) {
	return ext2_truncate_inode((ext2_fs_instance_t*)inode->i_instance, inode->i_ino, off);
}

dentry_t *ext2_getroot(struct _fs_instance_t *instance) {
	return ((ext2_fs_instance_t*)instance)->root;
}

dentry_t* ext2_lookup(struct _fs_instance_t *instance, struct _dentry_t* dentry, const char * name) {
	int flags = 0; // XXX

	int inode = getinode_from_name((ext2_fs_instance_t*)instance, dentry->d_inode->i_ino, name);
	if (inode == -ENOENT && (flags & O_CREAT)) {
		//inode = ext2_mknod2((ext2_fs_instance_t*)instance, name, 00644 | 0x8000, 0); //FIXME!
	} else if (inode <= 0) {
		return NULL;
	} else if (flags & O_EXCL && flags & O_CREAT) {
		return NULL;
	}
	
	if (flags & O_TRUNC) {
		ext2_truncate_inode((ext2_fs_instance_t*)instance, inode, 0);
	}

	struct ext2_inode einode;
	read_inode((ext2_fs_instance_t*)instance, inode, &einode);

	dentry_t *d = kmalloc(sizeof(dentry_t));
	char *n = kmalloc(strlen(name) + 1);
	strcpy(n, name);
	d->d_name = (const char*)n;
	d->d_inode = ext2inode_2_inode(instance, inode, &einode);

	return d;
}

int ext2_close(open_file_descriptor *ofd) {
	if (ofd == NULL) {
		return -1;
	}
	kfree(ofd);
	return 0;
}
