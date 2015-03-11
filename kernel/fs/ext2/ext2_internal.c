#include "ext2_internal.h"
#include <klibc/string.h>
#include <kmalloc.h>
#include <kdirent.h>
#include <kerrno.h>
#include <clock.h>
#include <klog.h>

struct _open_file_operations_t ext2fs_fops = {.write = ext2_write, .read = ext2_read, .seek = ext2_seek, .ioctl = NULL, .open = NULL, .close = ext2_close, .readdir = ext2_readdir};

static int addr_inode_data(ext2_fs_instance_t *instance, int inode, int n_blk);
static uint32_t alloc_block(ext2_fs_instance_t *instance);

#define max(a,b) ((a) > (b) ? (a) : (b))

static struct directories_t * readdir_inode(ext2_fs_instance_t *instance, int inode) {
	struct directories_t * dir_result = kmalloc(sizeof(struct directories_t));

	int addr_debut = addr_inode_data(instance, inode, 0);

	if (addr_debut == 0) {
		kfree(dir_result);
		return NULL;
	}

	struct ext2_directory *dir = kmalloc(sizeof(struct ext2_directory));
	instance->read_data(instance->super.device, dir, sizeof(struct ext2_directory), addr_debut);
	dir_result->dir = dir;
	dir_result->next = NULL;

	int cur_pos = addr_debut;
	while (dir->rec_len + cur_pos < addr_debut + (1024 << instance->superblock.s_log_block_size)) {
		cur_pos += dir->rec_len;
		dir = kmalloc(sizeof(struct ext2_directory));
		instance->read_data(instance->super.device, dir, sizeof(struct ext2_directory), cur_pos);
		struct directories_t *element = kmalloc(sizeof(struct directories_t));
		element->dir = dir;
		element->next = dir_result;
		dir_result = element;
	}

	return dir_result;
}

int getinode_from_name(ext2_fs_instance_t *instance, int inode, const char *name) {
	struct directories_t *dirs = readdir_inode(instance, inode);
	struct directories_t *aux = dirs;
	while (aux != NULL) {
		aux->dir->name[aux->dir->name_len] = '\0';
		if (strcmp(name, aux->dir->name) == 0) {
			return aux->dir->inode;
		}
		aux = aux->next;
	}
	return -ENOTDIR;
}

static inline struct ext2_inode* read_inode(ext2_fs_instance_t *instance, int inode) {
	if (inode > 0) {
		int n = (inode - 1) / instance->superblock.s_inodes_per_group;
		uint32_t ib = (inode - 1) % instance->superblock.s_inodes_per_group;
		
		if (instance->group_desc_table_internal[n].inode_bitmap[ib / 8] & (1 << (ib % 8))) {
			return &(instance->group_desc_table_internal[n].inodes[ib]);
		}
	}
	return NULL;
}

static int write_inode(ext2_fs_instance_t *instance, int inode, struct ext2_inode* einode) {
	if (inode > 0) {
		int n = (inode - 1) / instance->superblock.s_inodes_per_group;
		int ib = (inode - 1) % instance->superblock.s_inodes_per_group;
		
		int addr_table = instance->group_desc_table[n].bg_inode_table;
		
		// update hardware
		instance->write_data(instance->super.device, einode, sizeof(struct ext2_inode), addr_table * (1024 << instance->superblock.s_log_block_size) + sizeof(struct ext2_inode) * ib);
		// update software
		if (instance->group_desc_table_internal[n].inode_bitmap[ib / 8] & (1 << (ib % 8))) {
			if (&(instance->group_desc_table_internal[n].inodes[ib]) != einode) {
				instance->group_desc_table_internal[n].inodes[ib] = *einode;
			}
		} else {
			kerr("This inode is not used yet. Start allocating it!");
		}

		return 0;
	}
	return -ENOENT;
}

static void set_block_inode_data(ext2_fs_instance_t *instance, struct ext2_inode *einode, int blk_n, uint32_t blk) {
	if (blk_n < 12) { // Direct
		einode->i_block[blk_n] = blk;
		//write_inode(instance, einode->, einode); //XXX !!!!!
	} else if (blk_n < 12 + (1024 << instance->superblock.s_log_block_size) / 4) { // Indirect
		if (einode->i_block[12] == 0) {
			einode->i_block[12] = alloc_block(instance);
			//write_inode(instance, einode->, einode); //XXX !!!!!
		}
		int j = blk_n - 12; // Indice dans ce système indirect.
		uint32_t addr_0 = einode->i_block[12]; // @ du block d'adresses
		instance->write_data(instance->super.device, &blk, 4, addr_0 * (1024 << instance->superblock.s_log_block_size) + 4 * j);

	} else if (blk_n < 12 + (1024 << instance->superblock.s_log_block_size) / 4 + (1024 << instance->superblock.s_log_block_size) * (1024 << instance->superblock.s_log_block_size) / 16) { // Double indirect
		// TODO.
	} else {
		// TODO.
	}
}

static uint32_t addr_inode_data2(ext2_fs_instance_t *instance, struct ext2_inode *einode, int blk_n) {
	if (blk_n < 12) { // Direct
		return einode->i_block[blk_n] * (1024 << instance->superblock.s_log_block_size);
	} else if (blk_n < 12 + (1024 << instance->superblock.s_log_block_size) / 4) { // Indirect
		if (einode->i_block[12]) {
			int j = blk_n - 12; // Indice dans ce système indirect.
			uint32_t addr_0 = einode->i_block[12]; // @ du block d'adresses
			uint32_t addr_1;
			instance->read_data(instance->super.device, &addr_1, 4, addr_0 * (1024 << instance->superblock.s_log_block_size) + 4 * j);
			return addr_1 * (1024 << instance->superblock.s_log_block_size);
		}
	} else if (blk_n < 12 + (1024 << instance->superblock.s_log_block_size) / 4 + (1024 << instance->superblock.s_log_block_size) * (1024 << instance->superblock.s_log_block_size) / 16) { // Double indirect
		if (einode->i_block[13]) {
			int j = blk_n - 12 - (1024 << instance->superblock.s_log_block_size) / 4; // Indice dans ce système double-indirect.
			int k = j / ((1024 << instance->superblock.s_log_block_size) / 4); // Indice dans le premier block.
			int l = j % ((1024 << instance->superblock.s_log_block_size) / 4); // Indice dans le second block.

			uint32_t addr_0 = einode->i_block[13]; // @ du premier block d'adresses
			uint32_t addr_1; // @ du second block d'adresses
			instance->read_data(instance->super.device, &addr_1, 4, addr_0 * (1024 << instance->superblock.s_log_block_size) + 4 * k);
			uint32_t addr_2;
			instance->read_data(instance->super.device, &addr_2, 4, addr_1 * (1024 << instance->superblock.s_log_block_size) + 4 * l);
			return addr_2 * (1024 << instance->superblock.s_log_block_size);
		}
	} else { // Triple indirect !!11!!1
		if (einode->i_block[14]) {
			// TODO :D
		}
	}
	return 0;
}


static int addr_inode_data(ext2_fs_instance_t *instance, int inode, int n_blk) {
	struct ext2_inode *einode = read_inode(instance, inode);
	if (einode) {
		return addr_inode_data2(instance, einode, n_blk);
	}
	return 0;
}

static int getattr_inode(ext2_fs_instance_t *instance, int inode, struct stat *stbuf) {
	struct ext2_inode *einode = read_inode(instance, inode);
	if (einode) {
		stbuf->st_ino = inode + 1;
		stbuf->st_mode = einode->i_mode;
		stbuf->st_nlink = einode->i_links_count;
		stbuf->st_uid = einode->i_uid;
		stbuf->st_gid = einode->i_gid;
		stbuf->st_size = einode->i_size;
		stbuf->st_blksize = 1024 << instance->superblock.s_log_block_size;
		stbuf->st_blocks = einode->i_size / 512;
		stbuf->st_atime = einode->i_atime;
		stbuf->st_mtime = einode->i_mtime;
		stbuf->st_ctime = einode->i_ctime;
		return 0;
	}
	return -1;
}

static void setattr_inode(ext2_fs_instance_t *instance, int inode, struct stat *stbuf) {
	struct ext2_inode *einode = read_inode(instance, inode);
	if (einode) {
		einode->i_mode = stbuf->st_mode;
		einode->i_uid = stbuf->st_uid;
		einode->i_gid = stbuf->st_gid;
		einode->i_atime = stbuf->st_atime;
		einode->i_mtime = stbuf->st_mtime;
		einode->i_ctime = get_date();
		write_inode(instance, inode, einode);
	}
}

static uint32_t alloc_block(ext2_fs_instance_t *instance) {
	int i;
	for (i = 0; i < instance->n_groups; i++) {
		if (instance->group_desc_table[i].bg_free_blocks_count) {
			instance->group_desc_table[i].bg_free_blocks_count--;
			// TODO: decrement bg_free_blocks_count
			int addr_bitmap = instance->group_desc_table[i].bg_block_bitmap;
			uint8_t *block_bitmap = kmalloc(1024 << instance->superblock.s_log_block_size);
			instance->read_data(instance->super.device, block_bitmap, (1024 << instance->superblock.s_log_block_size), addr_bitmap * (1024 << instance->superblock.s_log_block_size));

			uint32_t ib;
			for (ib = 0; ib < instance->superblock.s_blocks_per_group; ib++) {
				if ((block_bitmap[ib/8] & (1 << (ib % 8))) == 0) {
					block_bitmap[ib/8] |= (1 << (ib % 8));
					instance->write_data(instance->super.device, &(block_bitmap[ib/8]), sizeof(uint8_t), addr_bitmap * (1024 << instance->superblock.s_log_block_size) + ib / 8);
					//kprintf("allocation block %d\n", ib + i * instance->superblock.s_blocks_per_group);
					return 1 + ib + i * instance->superblock.s_blocks_per_group;
				}
			}
		}
	}
	kerr("block allocation failed.");
	return 0;
}

static int alloc_inode(ext2_fs_instance_t *instance, struct ext2_inode *inode) {
	int i;
	for (i = 0; i < instance->n_groups; i++) {
		if (instance->group_desc_table[i].bg_free_inodes_count) {
			uint8_t *inode_bitmap = instance->group_desc_table_internal[i].inode_bitmap;

			instance->group_desc_table[i].bg_free_inodes_count--;
			// TODO: decrement bg_free_inodes_count
			
			int addr_bitmap = instance->group_desc_table[i].bg_inode_bitmap;
			int addr_table = instance->group_desc_table[i].bg_inode_table;

			uint32_t ib;
			for (ib = 0; ib < instance->superblock.s_inodes_per_group; ib++) {
				int inode_n = i * instance->superblock.s_inodes_per_group + ib + 1;
				if ((inode_bitmap[ib/8] & (1 << (ib % 8))) == 0) {
					memcpy(&instance->group_desc_table_internal[i].inodes[ib], inode, sizeof(struct ext2_inode));
					instance->write_data(instance->super.device, inode, sizeof(struct ext2_inode), addr_table * (1024 << instance->superblock.s_log_block_size) + sizeof(struct ext2_inode) * ib);
					inode_bitmap[ib/8] |= (1 << (ib % 8));
					instance->write_data(instance->super.device, &(inode_bitmap[ib/8]), sizeof(uint8_t), addr_bitmap * (1024 << instance->superblock.s_log_block_size) + ib / 8);
					return inode_n;
				}
			}
		}
	}
	kerr("inode allocation failed.");
	return 0;
}

static int alloc_block_inode(ext2_fs_instance_t *instance, int inode) {
	struct ext2_inode *einode = read_inode(instance, inode);
	if (einode) {
		int i = 0;
		while (einode->i_block[i] > 0 && i < 12) i++;
		if (i < 12) {
			einode->i_block[i] = alloc_block(instance);
			write_inode(instance, inode, einode);
			return einode->i_block[i];
		}
	}
	kerr("block inode allocation failed.");
	return 0;
}

static void free_block(ext2_fs_instance_t *instance, uint32_t blk) {
	//kprintf("libération block %d\n", blk);
	int i = (blk - 1) / instance->superblock.s_blocks_per_group; // Groupe de block.
	int ib = (blk - 1) - instance->superblock.s_blocks_per_group * i; // Indice dans le groupe.
	int addr_bitmap = instance->group_desc_table[i].bg_block_bitmap;

	uint8_t block_bitmap;
	instance->read_data(instance->super.device, &(block_bitmap), sizeof(uint8_t), addr_bitmap * (1024 << instance->superblock.s_log_block_size) + ib / 8);

	if (!(block_bitmap & (1 << (ib % 8)))) {
		kerr("block deja freed.");
	}

	block_bitmap &= ~(1 << (ib % 8));
	instance->write_data(instance->super.device, &(block_bitmap), sizeof(uint8_t), addr_bitmap * (1024 << instance->superblock.s_log_block_size) + ib / 8);
	
	instance->group_desc_table[i].bg_free_blocks_count++;
	// TODO: increment bg_free_blocks_count (ie, le faire sur le fs et pas juste en mémoire)
}

static void free_inode(ext2_fs_instance_t *instance, int inode) {
	struct ext2_inode *einode = read_inode(instance, inode);
	int i;
	for (i = 0; einode->i_block[i]; i++) {
		uint32_t blk = einode->i_block[i];
		free_block(instance, blk);
	}

	i = (inode - 1) / instance->superblock.s_inodes_per_group; // Groupe
	int ib = (inode - 1) - instance->superblock.s_inodes_per_group * i; // Indice dans le groupe.
	int addr_bitmap = instance->group_desc_table[i].bg_inode_bitmap;

	uint8_t *inode_bitmap = instance->group_desc_table_internal[i].inode_bitmap;

	if ((inode_bitmap[ib/8] & (1 << (ib % 8))) == 0) {
		kerr("inode deja freed.");
		return;
	}

	inode_bitmap[ib/8] &= ~(1 << (ib % 8));
	instance->write_data(instance->super.device, &(inode_bitmap[ib/8]), sizeof(uint8_t), addr_bitmap * (1024 << instance->superblock.s_log_block_size) + ib / 8);

	instance->group_desc_table[i].bg_free_inodes_count++;
	// TODO: ecrire sur le fs l'increment.
}

static void add_dir_entry(ext2_fs_instance_t *instance, int inode, const char *name, int type, int n_inode) {
	int addr_debut = addr_inode_data(instance, inode, 0);

	if (addr_debut == 0) {
		// TODO: create block. Ne devrait pas arriver...
	}

	struct ext2_directory *dir = kmalloc(sizeof(struct ext2_directory));
	instance->read_data(instance->super.device, dir, sizeof(struct ext2_directory), addr_debut);

	int cur_pos = addr_debut;
	while (dir->rec_len + cur_pos < addr_debut + (1024 << instance->superblock.s_log_block_size)) {
		cur_pos += dir->rec_len;
		instance->read_data(instance->super.device, dir, sizeof(struct ext2_directory), cur_pos);
	}

	int s = 4 + 2 + 1 + 1 + dir->name_len;
	s += (4 - (s % 4)) % 4;
	dir->rec_len = s;
	instance->write_data(instance->super.device, dir, s, cur_pos);
	cur_pos += s;
	
	struct ext2_directory ndir;
	ndir.inode = n_inode;
	ndir.rec_len = addr_debut + (1024 << instance->superblock.s_log_block_size) - cur_pos;
	ndir.name_len = strlen(name);
	ndir.file_type = type;
	strcpy(ndir.name, name);
	int s2 = 4 + 2 + 1 + 1 + ndir.name_len;
	s2 += (4 - (s2 % 4)) % 4;
	instance->write_data(instance->super.device, &ndir, s2, cur_pos);
}

static void remove_dir_entry(ext2_fs_instance_t *instance, int inode, const char *name) {
	int addr_debut = addr_inode_data(instance, inode, 0);

	if (addr_debut == 0) {
		return;
	}

	struct ext2_directory dir, dir2;
	instance->read_data(instance->super.device, &dir, sizeof(struct ext2_directory), addr_debut);

	int cur_pos = addr_debut;
	int cur_pos2 = 0;
	int dec = 0;
	while (dir.rec_len + cur_pos < addr_debut + (1024 << instance->superblock.s_log_block_size)) {
		dir.name[dir.name_len] = '\0';
		if (strcmp(name, dir.name) == 0) {
			dec = dir.rec_len;
		}

		cur_pos2 = cur_pos;
		cur_pos += dir.rec_len;
		memcpy(&dir2, &dir, sizeof(dir));
		instance->read_data(instance->super.device, &dir, sizeof(struct ext2_directory), cur_pos);
		if (dec > 0) {
			if (dir.rec_len + cur_pos >= addr_debut + (1024 << instance->superblock.s_log_block_size)) {
				dir.rec_len += dec;
			}
			instance->write_data(instance->super.device, &dir, dir.rec_len, cur_pos - dec); //XXX
		}
	}

	if (dec == 0 && cur_pos2) {
		dir.name[dir.name_len] = '\0';
		if (strcmp(name, dir.name) == 0) {
			dir2.rec_len += dir.rec_len;
			instance->write_data(instance->super.device, &dir2, dir2.rec_len, cur_pos2); //XXX
		}
	}
}

static void init_dir(ext2_fs_instance_t *instance, int inode, int parent_inode) {
	int addr = addr_inode_data(instance, inode, 0);

	if (addr == 0) {
		if (alloc_block_inode(instance, inode) >= 0) {
			addr = addr_inode_data(instance, inode, 0);
		} else {
			return;
		}
	}
	struct ext2_directory dir;
	dir.inode = inode;
  // inode (4) + rec_len (2) + name_len (1) + file_type (1) + name (1) + padding
	dir.rec_len = 4 + 2 + 1 + 1 + 4;
	dir.name_len = 1;
	dir.file_type = EXT2_FT_DIR;
	strcpy(dir.name, ".");

	instance->write_data(instance->super.device, &dir, sizeof(dir), addr);
	addr += dir.rec_len;

	dir.inode = parent_inode;
	dir.rec_len = (1024 << instance->superblock.s_log_block_size) - addr;
	dir.name_len = 2;
	dir.file_type = EXT2_FT_DIR;
	strcpy(dir.name, "..");
	instance->write_data(instance->super.device, &dir, sizeof(dir), addr);
}


static int mknod_inode(ext2_fs_instance_t *instance, int inode, const char *name, mode_t mode, dev_t dev __attribute__((unused))) {
	struct ext2_inode n_inode;
	memset(&n_inode, 0, sizeof(struct ext2_inode));
	n_inode.i_mode = mode;
	n_inode.i_links_count = 1;
// TODO uid/gid
//	n_inode.i_uid = fc->uid;
//	n_inode.i_gid = fc->gid;
	time_t cdate = get_date();
	n_inode.i_atime = cdate;
	n_inode.i_ctime = cdate;
	n_inode.i_mtime = cdate;
	
	int i = alloc_inode(instance, &n_inode);
	if (mode & EXT2_S_IFDIR) {
		add_dir_entry(instance, inode, name, EXT2_FT_DIR, i);
		init_dir(instance, i, inode);
	} else if (mode & EXT2_S_IFIFO) {
		add_dir_entry(instance, inode, name, EXT2_FT_FIFO, i);
	} else if (mode & EXT2_S_IFCHR) {
		add_dir_entry(instance, inode, name, EXT2_FT_CHRDEV, i);
	} else if (mode & EXT2_S_IFBLK) {
		add_dir_entry(instance, inode, name, EXT2_FT_BLKDEV, i);
	} else if (mode & EXT2_S_IFSOCK) {
		add_dir_entry(instance, inode, name, EXT2_FT_SOCK, i);
	} else if (mode & EXT2_S_IFLNK) {
		add_dir_entry(instance, inode, name, EXT2_FT_SYMLINK, i);
	} else {
		add_dir_entry(instance, inode, name, EXT2_FT_REG_FILE, i);
	}
	return i;
}

static void ext2inode_2_inode(inode_t* inode, struct _fs_instance_t *instance, int ino, struct ext2_inode *einode) {
	if (einode == NULL) {
		kerr("inode is null! Can't convert it to real inode.");
		return;
	}
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
	inode->i_fs_specific = NULL;
}

dentry_t * init_rootext2fs(ext2_fs_instance_t *instance) {
	dentry_t *root_ext2fs = kmalloc(sizeof(dentry_t));
	
	root_ext2fs->d_name = "";

	struct ext2_inode *einode = read_inode(instance, EXT2_ROOT_INO);
	root_ext2fs->d_inode = kmalloc(sizeof(inode_t));
	ext2inode_2_inode(root_ext2fs->d_inode, (fs_instance_t*)instance, EXT2_ROOT_INO, einode);
	root_ext2fs->d_inode->i_count = 0;
	root_ext2fs->d_pdentry = NULL;

	return root_ext2fs;
}

static ssize_t ext2_write2inode(ext2_fs_instance_t *instance, int inode, struct ext2_inode *einode, unsigned int offset, const void *buf, size_t size) {
	if (einode != NULL) {
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

}
