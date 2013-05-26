/**
 * @file ext2_functions.c
 *
 * @author TacOS developers 
 *
 * @section LICENSE
 *
 * Copyright (C) 2010, 2011, 2012, 2013 - TacOS developers.
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

int ext2_rename(inode_t *old_dir, dentry_t *old_dentry, inode_t *new_dir, dentry_t *new_dentry) {
	// Remove inode from parent dir.
	remove_dir_entry((ext2_fs_instance_t*)old_dir->i_instance, old_dir->i_ino, old_dentry->d_name);

	add_dir_entry((ext2_fs_instance_t*)new_dir->i_instance, new_dir->i_ino, new_dentry->d_name, EXT2_FT_REG_FILE, old_dentry->d_inode->i_ino); //XXX

	return 0;
}

int ext2_rmdir(inode_t *dir, dentry_t *dentry) {
	if (S_ISDIR(dentry->d_inode->i_mode)) {
		remove_dir_entry((ext2_fs_instance_t*)dir->i_instance, dir->i_ino, dentry->d_name);
		return 0;
	} else {
		return -ENOTDIR;
	}


	// TODO: Check is empty.
	// TODO: Call rmdir_inode!
	// décompter link!
}

//XXX: Une partie pourrait être générique je pense. 
int ext2_setattr(inode_t *inode, file_attributes_t *attr) {
	struct stat s;
	if (getattr_inode((ext2_fs_instance_t*)inode->i_instance, inode->i_ino, &s) == 0) {
		if (attr->mask & ATTR_UID) {
			s.st_uid = attr->stbuf.st_uid;
		}
		if (attr->mask & ATTR_GID) {
			s.st_gid = attr->stbuf.st_gid;
		}
		if (attr->mask & ATTR_MODE) {
			s.st_mode = attr->stbuf.st_mode;
		}
		if (attr->mask & ATTR_ATIME) {
			s.st_atime = attr->stbuf.st_atime;
		}
		if (attr->mask & ATTR_MTIME) {
			s.st_mtime = attr->stbuf.st_mtime;
		}
		if (attr->mask & ATTR_CTIME) {
			s.st_ctime = attr->stbuf.st_ctime;
		}
		if (attr->mask & ATTR_SIZE && attr->ia_size != inode->i_size) {
			ext2_truncate(inode, attr->ia_size);
		}
		setattr_inode((ext2_fs_instance_t*)inode->i_instance, inode->i_ino, &s);
		return 0;
	}
	return -1;
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
	int inode = ofd->inode->i_ino;
	if (inode >= 0) {
		size_t c = 0;
		struct directories_t *dirs = readdir_inode(instance, inode);
		struct directories_t *aux = dirs;

		while (c < ofd->current_octet && aux != NULL) {
			aux = aux->next;
			c++;
		}

		struct dirent *d;
		while (aux != NULL && count + sizeof(d->d_ino) + sizeof(d->d_reclen) + sizeof(d->d_type) + aux->dir->name_len + 1 < size) {
			d = (struct dirent *)(entries + count);
			d->d_ino = aux->dir->inode;
			strncpy(d->d_name, aux->dir->name, aux->dir->name_len);
			d->d_name[aux->dir->name_len] = '\0';
			d->d_reclen = sizeof(d->d_ino) + sizeof(d->d_reclen) + sizeof(d->d_type) + aux->dir->name_len + 1;
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
		break;
	case SEEK_CUR:
		if (ofd->current_octet + (uint32_t)offset > ofd->file_size) {
			return -1;
		}
		ofd->current_octet += offset;
		break;
	case SEEK_END:
		if ((uint32_t)offset > ofd->file_size) {
			return -1;
		}
		ofd->current_octet = ofd->file_size - offset;
		break;
	}

	return 0;
}

ssize_t ext2_write(open_file_descriptor * ofd, const void *buf, size_t size) {
	if ((ofd->flags & O_ACCMODE) == O_RDONLY) {
		return 0;
	}

	int inode = ofd->inode->i_ino;
	if (inode >= 0) {
		ext2_fs_instance_t *instance = (ext2_fs_instance_t*) ofd->fs_instance;
		
		struct ext2_inode *einode = read_inode(instance, inode);
		if (einode != NULL) {
			unsigned int offset;
			if (ofd->flags & O_APPEND) {
				offset = einode->i_size;
			} else {
			 	offset = ofd->current_octet;
			}

			int count = 0;

			// On avance de block en block tant que offset > taille d'un block.
			int n_blk = 0;
			int off = offset;
			while (off >= (1024 << instance->superblock.s_log_block_size)) {
				if (addr_inode_data2(instance, einode, n_blk) == 0) {
					int addr = alloc_block(instance);
					set_block_inode_data(instance, einode, n_blk, addr);
				}

				off -= 1024 << instance->superblock.s_log_block_size;
				n_blk++;
			}

			while (size > 0) {
				int addr = addr_inode_data2(instance, einode, n_blk);
				if (addr == 0) {
					addr = alloc_block(instance);
					set_block_inode_data(instance, einode, n_blk, addr);
					addr *= (1024 << instance->superblock.s_log_block_size);
				}
				addr += off;

				size_t size2 = (1024 << instance->superblock.s_log_block_size) - off;
				off = 0;
				if (size2 > size) {
					size2 = size;
				}
				instance->write_data(instance->super.device, ((char*)buf) + count, size2, addr);
				size -= size2;
				count += size2;
				n_blk++;
			}

			einode->i_size = max(einode->i_size, offset + count);
	//		struct timeval tv;
	//		gettimeofday(&tv, NULL);
	//		einode.i_mtime = tv.tv_sec;
			write_inode(instance, inode, einode);
			return count;		
		} else {
			return -ENOENT;
		}
		return size;
	} else {
		return inode;
	}
}

ssize_t ext2_read(open_file_descriptor * ofd, void * buf, size_t size) {
	if ((ofd->flags & O_ACCMODE) == O_WRONLY) {
		return 0;
	}
	int inode = ofd->inode->i_ino;
	if (inode >= 0) {
		ext2_fs_instance_t *instance = (ext2_fs_instance_t*) ofd->fs_instance;
		off_t offset = ofd->current_octet;
		int count = 0;
		struct ext2_inode *einode = read_inode(instance, inode);
		
		if (offset >= einode->i_size) {
			return 0;
		}

		if (size + offset > einode->i_size) {
			size = einode->i_size - offset;
		}
		
		int n_blk = 0;
		while (offset >= (unsigned int)(1024 << instance->superblock.s_log_block_size)) {
			n_blk++;
			offset -= (1024 << instance->superblock.s_log_block_size);
		}
		//int n_blk = offset / (1024 << instance->superblock.s_log_block_size);
		//offset %= (1024 << instance->superblock.s_log_block_size);
		//
		while (size > 0) {
			int addr = addr_inode_data2(instance, einode, n_blk);
			if (addr == 0) break;
			n_blk++;

			size_t size2 = (1024 << instance->superblock.s_log_block_size) - offset;
			if (size2 > size) {
				size2 = size;
			}

			instance->read_data(instance->super.device, ((char*)buf) + count, size2, addr + offset);

			size -= size2;
			count += size2;
			offset = 0;
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


int ext2_mknod(inode_t *dir, dentry_t *dentry, mode_t mode, dev_t dev) {
	int ino =  mknod_inode((ext2_fs_instance_t*)dir->i_instance, dir->i_ino, dentry->d_name, mode, dev);
	if (ino == 0) {
		return -ENOTDIR; //XXX
	}
	struct ext2_inode *einode = read_inode((ext2_fs_instance_t*)dir->i_instance, ino);

	dentry->d_inode = kmalloc(sizeof(inode_t));
	ext2inode_2_inode(dentry->d_inode, dir->i_instance, ino, einode);
	dentry->d_inode->i_count = 0;

	return 0;
}

int ext2_truncate(inode_t *inode, off_t off) {

	uint32_t size = off;
	ext2_fs_instance_t *instance = (ext2_fs_instance_t*)inode->i_instance;
	struct ext2_inode *einode = read_inode(instance, inode->i_ino);
	if (einode) {
// TODO vérifier le bon fonctionnement.
		int n_blk;
		if (size > 0) {
			n_blk	= (size - 1) / (1024 << instance->superblock.s_log_block_size) + 1;
		} else {
			n_blk = 1;
		}

		int addr = addr_inode_data2(instance, einode, n_blk);
		if (addr) {
			// 1er cas : off est plus petit que la taille du fichier.
			while (addr) {
				if (off <= 0) {
					free_block(instance, addr / (1024 << instance->superblock.s_log_block_size));
				} else {
					off -= 1024 << instance->superblock.s_log_block_size;
				}
				n_blk++;
				addr = addr_inode_data2(instance, einode, n_blk);
			}
		} else {
			// 2er cas : off est plus grand.
			while (off > 0) {
				set_block_inode_data(instance, einode, n_blk, alloc_block(instance));
				n_blk++;
				off -= 1024 << instance->superblock.s_log_block_size;
			}
		}

		einode->i_size = size;
		write_inode(instance, inode->i_ino, einode);
		ext2inode_2_inode(inode, inode->i_instance, inode->i_ino, einode);
		return 0;
	}
	return -ENOENT;
}

dentry_t *ext2_getroot(struct _fs_instance_t *instance) {
	return ((ext2_fs_instance_t*)instance)->root;
}

dentry_t* ext2_lookup(struct _fs_instance_t *instance, struct _dentry_t* dentry, const char * name) {
	int flags = 0; // XXX

	int inode = getinode_from_name((ext2_fs_instance_t*)instance, dentry->d_inode->i_ino, name);
	if (inode == -ENOENT && (flags & O_CREAT)) {
		//inode = ext2_mknod2((ext2_fs_instance_t*)instance, name, 00644 | 0x8000, 0); //FIXME!
		// Plusieurs choses : getinode_from_name retourne ENOTDIR et pas ENOENT, 
		// flags vaut 0, et ce mknod est réalisé plus haut, peut-être qu'on devrait
		// supprimer ce cas tout simplement ?
	} else if (inode <= 0) {
		return NULL;
	} else if (flags & O_EXCL && flags & O_CREAT) {
		return NULL;
	}
	
	struct ext2_inode *einode = read_inode((ext2_fs_instance_t*)instance, inode);

	dentry_t *d = kmalloc(sizeof(dentry_t));
	char *n = strdup(name);
	d->d_name = (const char*)n;
	d->d_pdentry = dentry;
	d->d_inode = kmalloc(sizeof(inode_t));
	ext2inode_2_inode(d->d_inode, instance, inode, einode);
	d->d_inode->i_count = 0;

	if (flags & O_TRUNC) {
		ext2_truncate(d->d_inode, 0);
	}

	return d;
}

int ext2_close(open_file_descriptor *ofd) {
	if (ofd == NULL) {
		return -1;
	}
	kfree(ofd);
	return 0;
}
