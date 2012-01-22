/**
 * @file ext2_functions.c
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
 * Description de ce que fait le fichier
 */

#include "ext2_internal.c"

#include <klog.h>
#include <types.h>
#include <kfcntl.h>

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

static int ext2_rmdir(fs_instance_t *instance, const char * path) {
	int inode = getinode_from_path((ext2_fs_instance_t*)instance, path);
	if (inode > 0) {
		char filename[256];
		char * dir = kmalloc(strlen(path));
		split_dir_filename(path, dir, filename);
		inode = getinode_from_path(dir);
		// TODO: Check is dir.
		// TODO: Call rmdir_inode!
		remove_dir_entry(inode, filename);
		// Free !
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

int ext2_opendir(fs_instance_t *instance, const char * path) {
	int inode = getinode_from_path((ext2_fs_instance_t*)instance, path);

	if (inode < 0) {
		return inode;
	}
	return 0;
}

int ext2_readdir(fs_instance_t *instance, const char * path, int iter, char * filename) {
	ext2_fs_instance_t* inst = (ext2_fs_instance_t*)instance;
	int inode = getinode_from_path(inst, path);
	if (inode >= 0) {
		struct directories_t *dirs = readdir_inode(inst, inode);
		struct directories_t *aux = dirs;
		while (aux != NULL) {
			if (!iter) {
				strncpy(filename, aux->dir->name, aux->dir->name_len);
				filename[aux->dir->name_len] = '\0';
				return 0;
			}
			aux = aux->next;
			iter--;
		}
		return -1;
	} else {
		return inode;
	}
}

int ext2_seek(open_file_descriptor * ofd, long offset, int whence) {
	ext2_fs_instance_t *instance = (ext2_fs_instance_t*) ofd->fs_instance;
	int inode = ((ext2_extra_data*)ofd->extra_data)->inode;
	struct ext2_inode einode;
	read_inode(instance, inode, &einode);
	switch (whence) {
	case SEEK_SET:
		if ((uint32_t)offset > einode.i_size) {
			return -1;
		}
		ofd->current_octet = offset;
		break;
	case SEEK_CUR:
		if (ofd->current_octet + (uint32_t)offset > einode.i_size) {
			return -1;
		}
		ofd->current_octet += offset;
		break;
	case SEEK_END:
		if ((uint32_t)offset > einode.i_size) {
			return -1;
		}
		ofd->current_octet = einode.i_size - offset;
		break;
	}

	return 0;
}

size_t ext2_write (open_file_descriptor * ofd, const void *buf, size_t size) {
	int inode = ((ext2_extra_data*)ofd->extra_data)->inode;
	if (inode >= 0) {
		ext2_fs_instance_t *instance = (ext2_fs_instance_t*) ofd->fs_instance;
		unsigned int offset = ofd->current_octet;
		struct ext2_inode einode;
		if (read_inode(instance, inode, &einode) == 0) {
			int count = 0;
			struct blk_t *last = NULL;
			struct blk_t *blocks = addr_inode_data(instance, inode);
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
	int inode = ((ext2_extra_data*)ofd->extra_data)->inode;
	if (inode >= 0) {
		ext2_fs_instance_t *instance = (ext2_fs_instance_t*) ofd->fs_instance;
		off_t offset = ofd->current_octet;
		int count = 0;
		struct ext2_inode einode;
		read_inode(instance, inode, &einode);
		
		if (offset >= einode.i_size) {
			return 0;
		}

		if (size + offset > einode.i_size) {
			size = einode.i_size - offset;
		}

		struct blk_t *blocks = addr_inode_data(instance, inode);
		if (blocks == NULL) return 0;
		while (offset >= (unsigned int)(1024 << instance->superblock.s_log_block_size)) {
			if (blocks && blocks->next) {
				blocks = blocks->next;
			} else return 0;
			offset -= 1024 << instance->superblock.s_log_block_size;
		}


		while (size > 0 && blocks != NULL) {
			int addr = blocks->addr + offset;
			size_t size2 = (1024 << instance->superblock.s_log_block_size) - offset;
			offset = 0;
			if (size2 > size) {
				size2 = size;
			}

			instance->read_data(instance->super.device, ((char*)buf) + count, size2, addr);
			size -= size2;
			count += size2;

			blocks = blocks->next;
		}

		// Commenté pour des raisons évidentes de perf.
		//	struct timeval tv;
		//	gettimeofday(&tv, NULL);
		//	einode.i_atime = tv.tv_sec;
		//	write_inode(inode, &einode);
 		
		ofd->current_octet += count;
		return count;
	} else {
		return inode;
	}
}

int ext2_mknod(fs_instance_t *instance, const char * path, mode_t mode, dev_t dev) {
	char filename[256];
  char * dir = kmalloc(strlen(path));
	split_dir_filename(path, dir, filename);

	int inode = getinode_from_path((ext2_fs_instance_t*)instance, dir);
	if (inode >= 0) {
		mknod_inode((ext2_fs_instance_t*)instance, inode, filename, mode, dev);
		return 0;
	}

	return -ENOENT;
}

open_file_descriptor * ext2_open(fs_instance_t *instance, const char * path, uint32_t flags) {
	klog("open %s\n", path);
	int inode = getinode_from_path((ext2_fs_instance_t*)instance, path);
	if (inode == -ENOENT && (flags & O_CREAT)) {
		//XXX
		ext2_mknod(instance, path, 00644 | 0x8000, 0);
	} else if (inode <= 0) {
		return NULL;
	} else if (flags & O_EXCL && flags & O_CREAT) {
		return NULL;
	}
	
	open_file_descriptor *ofd = kmalloc(sizeof(open_file_descriptor));
	ofd->fs_instance = instance;
	ofd->current_octet = 0;
	//ofd->file_size = f->size;
	ofd->read = ext2_read;
	ofd->write = ext2_write;
	ofd->seek = ext2_seek;

	ext2_extra_data *extra_data = kmalloc(sizeof(ext2_extra_data));
	extra_data->inode = inode;
	extra_data->type = EXT2_FT_REG_FILE; //XXX!
	ofd->extra_data = extra_data;

	return ofd;
}
