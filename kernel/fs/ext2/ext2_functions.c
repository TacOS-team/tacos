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

static dentry_t root_ext2fs;
static struct _open_file_operations_t ext2fs_fops = {.write = ext2_write, .read = ext2_read, .seek = ext2_seek, .ioctl = NULL, .open = NULL, .close = ext2_close, .readdir = ext2_readdir};

void init_rootext2fs() {
	root_ext2fs.d_name = "";
	root_ext2fs.d_inode = kmalloc(sizeof(inode_t));
	root_ext2fs.d_inode->i_ino = EXT2_ROOT_INO;
	ext2_extra_data *ext = kmalloc(sizeof(ext2_extra_data));
	ext->inode = EXT2_ROOT_INO;
	ext->type = EXT2_FT_DIR;
	ext->blocks = NULL;
  root_ext2fs.d_inode->i_fs_specific = ext;
	root_ext2fs.d_inode->i_mode = S_IFDIR | 00755;
	root_ext2fs.d_inode->i_fops = &ext2fs_fops;
}


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


int ext2_chmod(fs_instance_t *instance, const char * path, mode_t mode) {
	ext2_fs_instance_t* inst = (ext2_fs_instance_t*)instance;
	int inode = getinode_from_path(inst, path);
	if (inode >= 0) {
		struct stat s;
		getattr_inode(inst, inode, &s);
		s.st_mode = mode;
		setattr_inode(inst, inode, &s);
		return 0;
	} else {
		return inode;
	}
}

int ext2_chown(fs_instance_t *instance, const char * path, uid_t uid, gid_t gid) {
	ext2_fs_instance_t* inst = (ext2_fs_instance_t*)instance;
	int inode = getinode_from_path(inst, path);
	if (inode >= 0) {
		struct stat s;
		getattr_inode(inst, inode, &s);
		s.st_uid = uid;
		s.st_gid = gid;
		setattr_inode(inst, inode, &s);
		return 0;
	} else {
		return inode;
	}
}

int ext2_unlink(fs_instance_t *instance, const char * path) {
	int inode = getinode_from_path((ext2_fs_instance_t*)instance, path);
	if (inode > 0) {
		char filename[256];
		char * dir = kmalloc(strlen(path));
		split_dir_filename(path, dir, filename);
		inode = getinode_from_path((ext2_fs_instance_t*)instance, dir);
		// TODO: Check is regular file.
		remove_dir_entry((ext2_fs_instance_t*)instance, inode, filename);
		// Free !
		return 0;
	} else {
		return inode;
	}
}

int ext2_mkdir(fs_instance_t *instance, const char * path, mode_t mode) {
	char filename[256];
  char * dir = kmalloc(strlen(path));
	split_dir_filename(path, dir, filename);

	int inode = getinode_from_path((ext2_fs_instance_t*)instance, dir);
	if (inode >= 0) {
		mkdir_inode((ext2_fs_instance_t*)instance, inode, filename, mode);
		return 0;
	} else {
		return inode;
	}
}

int ext2_stat(fs_instance_t *instance, const char *path, struct stat *stbuf) {
	ext2_fs_instance_t* inst = (ext2_fs_instance_t*)instance;
	int inode = getinode_from_path(inst, path);
	if (inode >= 0) {
		getattr_inode(inst, inode, stbuf);
		return 0;
	} else {
		return inode;
	}
}


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

int ext2_mknod(fs_instance_t *instance, const char * path, mode_t mode, dev_t dev) {
	int inode = ext2_mknod2((ext2_fs_instance_t*)instance, path, mode, dev);
	if (inode >= 0) {
		return 0;
	}

	return -ENOENT;
}

int ext2_truncate(fs_instance_t *instance, const char * path, off_t off) {
	int inode = getinode_from_path((ext2_fs_instance_t*)instance, path);
	if (inode > 0) {
		return ext2_truncate_inode((ext2_fs_instance_t*)instance, inode, off);
	}
	return -ENOENT;
}

static inode_t* ext2inode_2_inode(struct _fs_instance_t *instance, int ino, struct ext2_inode *einode) {
	inode_t *inode = kmalloc(sizeof(inode_t));
	inode->i_ino = ino;
	inode->i_mode = einode->i_mode;
	inode->i_uid = einode->i_uid;
	inode->i_gid = einode->i_gid;
	inode->i_size = einode->i_size;
	inode->i_atime = einode->i_atime;
	inode->i_ctime = einode->i_ctime;
	inode->i_dtime = einode->i_dtime;
	inode->i_mtime = einode->i_mtime;
	inode->i_nlink = einode->i_links_count;
	inode->i_blocks = einode->i_blocks;
	inode->i_instance = instance;
	inode->i_fops = &ext2fs_fops; 
	ext2_extra_data *extra_data = kmalloc(sizeof(ext2_extra_data));
	extra_data->inode = ino;
	extra_data->type = EXT2_FT_REG_FILE; //XXX!!!
	extra_data->blocks = addr_inode_data((ext2_fs_instance_t*)instance, ino);
	inode->i_fs_specific = extra_data;

	return inode;
}

dentry_t *ext2_getroot() {
	return &root_ext2fs;
}

dentry_t* ext2_lookup(struct _fs_instance_t *instance, struct _dentry_t* dentry, const char * name) {
	int flags = 0; // XXX
	

	int inode = getinode_from_name((ext2_fs_instance_t*)instance, dentry->d_inode->i_ino, name);
	if (inode == -ENOENT && (flags & O_CREAT)) {
		inode = ext2_mknod2((ext2_fs_instance_t*)instance, name, 00644 | 0x8000, 0); //FIXME!
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
	d->d_name = kmalloc(strlen(name) + 1);
	strcpy(d->d_name, name);
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
