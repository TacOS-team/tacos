#include "ext2_internal.h"
#include <klibc/string.h>
#include <klog.h>
#include <kmalloc.h>
#include <kdirent.h>
#include <kerrno.h>
#include <clock.h>

struct _open_file_operations_t ext2fs_fops = {.write = ext2_write, .read = ext2_read, .seek = ext2_seek, .ioctl = NULL, .open = NULL, .close = ext2_close, .readdir = ext2_readdir};

static struct blk_t* addr_inode_data(ext2_fs_instance_t *instance, int inode);
static struct blk_t* addr_inode_data2(ext2_fs_instance_t *instance, struct ext2_inode *einode);
static int read_inode(ext2_fs_instance_t *instance, int inode, struct ext2_inode* einode);
static uint32_t alloc_block(ext2_fs_instance_t *instance);

#define max(a,b) ((a) > (b) ? (a) : (b))

static struct directories_t * readdir_inode(ext2_fs_instance_t *instance, int inode) {
	struct directories_t * dir_result = kmalloc(sizeof(struct directories_t));

	struct blk_t *blocks = addr_inode_data(instance, inode);

	if (blocks == NULL) {
		return NULL;
	}

	int addr_debut = blocks->addr;

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

static int read_inode(ext2_fs_instance_t *instance, int inode, struct ext2_inode* einode) {
	if (inode > 0) {
		uint32_t i = inode - 1;
		int n = i / instance->superblock.s_inodes_per_group;
		uint32_t ib = i % instance->superblock.s_inodes_per_group;
		
		int addr_bitmap = instance->group_desc_table[n].bg_inode_bitmap;
		int addr_table = instance->group_desc_table[n].bg_inode_table;
		uint8_t inode_bitmap;
		
		instance->read_data(instance->super.device, &inode_bitmap, 1, addr_bitmap * (1024 << instance->superblock.s_log_block_size) + ib / 8);
	
		if (inode_bitmap & (1 << (ib % 8))) {
			instance->read_data(instance->super.device, einode, sizeof(struct ext2_inode), addr_table * (1024 << instance->superblock.s_log_block_size) + sizeof(struct ext2_inode) * ib);
			return 0;
		}
	}
	return -ENOENT;
}

static int write_inode(ext2_fs_instance_t *instance, int inode, struct ext2_inode* einode) {
	if (inode > 0) {
		int i = inode - 1;
		int n = i / instance->superblock.s_inodes_per_group;
		int ib = i % instance->superblock.s_inodes_per_group;
		
		int addr_table = instance->group_desc_table[n].bg_inode_table;
		
		instance->write_data(instance->super.device, einode, sizeof(struct ext2_inode), addr_table * (1024 << instance->superblock.s_log_block_size) + sizeof(struct ext2_inode) * ib);
		return 0;
	}
	return -ENOENT;
}

static struct blk_t* read_indirect_blk(ext2_fs_instance_t *instance, struct blk_t* blocks, int addr) {
	int j;
	uint32_t *addrs = kmalloc(1024 << instance->superblock.s_log_block_size);
	instance->read_data(instance->super.device, addrs, 1024 << instance->superblock.s_log_block_size, addr * (1024 << instance->superblock.s_log_block_size));
	for (j = 0; j < (1024 << instance->superblock.s_log_block_size) / 4 && addrs[j]; j++) {
		struct blk_t* element = kmalloc(sizeof(struct blk_t));
		element->next = NULL;
		element->addr = addrs[j] * (1024 << instance->superblock.s_log_block_size);
		blocks->next = element;
		blocks = element;
	}
	return blocks;
}

static struct blk_t* read_double_indirect_blk(ext2_fs_instance_t *instance, struct blk_t* blocks, int addr) {
	int j;
	uint32_t *addrs = kmalloc(1024 << instance->superblock.s_log_block_size);
	instance->read_data(instance->super.device, addrs, 1024 << instance->superblock.s_log_block_size, addr * (1024 << instance->superblock.s_log_block_size));
	for (j = 0; j < (1024 << instance->superblock.s_log_block_size) / 4 && addrs[j]; j++) {
		blocks = read_indirect_blk(instance, blocks, addrs[j]);
	}
	return blocks;
}

static struct blk_t* read_triple_indirect_blk(ext2_fs_instance_t *instance, struct blk_t* blocks, int addr) {
	int j;
	uint32_t *addrs = kmalloc(1024 << instance->superblock.s_log_block_size);
	instance->read_data(instance->super.device, addrs, 1024 << instance->superblock.s_log_block_size, addr * (1024 << instance->superblock.s_log_block_size));
	for (j = 0; j < (1024 << instance->superblock.s_log_block_size) / 4 && addrs[j]; j++) {
		blocks = read_double_indirect_blk(instance, blocks, addrs[j]);
	}
	return blocks;
}


static void update_blocks(ext2_fs_instance_t *instance, struct ext2_inode* einode, struct blk_t *blocks) {
	int i = 0;
	int block_size = (1024 << instance->superblock.s_log_block_size);
	while (blocks) {
		if (i < 12) {
			einode->i_block[i] = blocks->addr / block_size;
		} else if (i >= 12 && i < 12 + block_size / 4) {
			if (einode->i_block[12] == 0) {
				einode->i_block[12] = alloc_block(instance);
				uint8_t zeros[1024];
				memset(zeros, 0, sizeof(zeros));
				int j;
				for (j = 0; j < (1 << instance->superblock.s_log_block_size); j++) {
					instance->write_data(instance->super.device, zeros, block_size, einode->i_block[12] * block_size + j * 1024);
				}
			}
			uint32_t addr = blocks->addr / block_size;
			instance->write_data(instance->super.device, &addr, 4, einode->i_block[12] * block_size + (i - 12) * 4); 
		} else if (i >= 12 + block_size / 4 && i < 12 + (block_size / 4) * (block_size / 4)) {
			if (einode->i_block[13] == 0) {
				einode->i_block[13] = alloc_block(instance);
			}
			//TODO!
		} else {
			if (einode->i_block[14] == 0) {
				einode->i_block[14] = alloc_block(instance);
			}
			//TODO!
		}
		blocks = blocks->next;
		i++;
	}
	while (i < 12) {
		einode->i_block[i] = 0;
		i++;
	}
	// TODO!
}

static struct blk_t* addr_inode_data2(ext2_fs_instance_t *instance, struct ext2_inode *einode) {
	struct blk_t *blocks = NULL;
	struct blk_t *last = NULL;
	int j;
	// direct blocks
	for (j = 0; j < 12 && einode->i_block[j]; j++) {
		struct blk_t* element = kmalloc(sizeof(struct blk_t));
		element->next = NULL;
		element->addr = einode->i_block[j] * (1024 << instance->superblock.s_log_block_size);
		if (last == NULL) {
			blocks = element;
		} else {
			last->next = element;
		}
		last = element;
	}
	if (j == 12) {
		// indirect blocks
		if (einode->i_block[12]) {
			last = read_indirect_blk(instance, last, einode->i_block[12]);
		}
		if (einode->i_block[13]) {
			last = read_double_indirect_blk(instance, last, einode->i_block[13]);
		}
		if (einode->i_block[14]) {
			last = read_triple_indirect_blk(instance, last, einode->i_block[14]);
		}
	}
	return blocks;
}

static struct blk_t* addr_inode_data(ext2_fs_instance_t *instance, int inode) {
	struct ext2_inode einode;
	if (read_inode(instance, inode, &einode) >= 0) {
		return addr_inode_data2(instance, &einode);
	}
	return NULL;
}

static int getattr_inode(ext2_fs_instance_t *instance, int inode, struct stat *stbuf) {
	struct ext2_inode einode;
	if (read_inode(instance, inode, &einode) == 0) {
		stbuf->st_ino = inode + 1;
		stbuf->st_mode = einode.i_mode;
		stbuf->st_nlink = einode.i_links_count;
		stbuf->st_uid = einode.i_uid;
		stbuf->st_gid = einode.i_gid;
		stbuf->st_size = einode.i_size;
		stbuf->st_blksize = 1024 << instance->superblock.s_log_block_size;
		stbuf->st_blocks = einode.i_size / 512;
		stbuf->st_atime = einode.i_atime;
		stbuf->st_mtime = einode.i_mtime;
		stbuf->st_ctime = einode.i_ctime;
		return 0;
	}
	return -1;
}

static void setattr_inode(ext2_fs_instance_t *instance, int inode, struct stat *stbuf) {
	struct ext2_inode einode;
	if (read_inode(instance, inode, &einode) == 0) {
		einode.i_mode = stbuf->st_mode;
		einode.i_uid = stbuf->st_uid;
		einode.i_gid = stbuf->st_gid;
		einode.i_atime = stbuf->st_atime;
		einode.i_mtime = stbuf->st_mtime;
		einode.i_ctime = get_date();
		write_inode(instance, inode, &einode);
	}
}

static uint32_t alloc_block(ext2_fs_instance_t *instance) {
	int i;
	for (i = 0; i < instance->n_groups; i++) {
		if (instance->group_desc_table[i].bg_free_blocks_count) {
			// TODO: decrement bg_free_blocks_count
			int addr_bitmap = instance->group_desc_table[i].bg_block_bitmap;
			uint8_t *block_bitmap = kmalloc(1024 << instance->superblock.s_log_block_size);
			instance->read_data(instance->super.device, block_bitmap, (1024 << instance->superblock.s_log_block_size), addr_bitmap * (1024 << instance->superblock.s_log_block_size));

			uint32_t ib;
			for (ib = 0; ib < instance->superblock.s_blocks_per_group; ib++) {
				if ((block_bitmap[ib/8] & (1 << (ib % 8))) == 0) {
					block_bitmap[ib/8] |= (1 << (ib % 8));
					instance->write_data(instance->super.device, &(block_bitmap[ib/8]), sizeof(uint8_t), addr_bitmap * (1024 << instance->superblock.s_log_block_size) + ib / 8);
					return ib + i * instance->superblock.s_blocks_per_group;
				}
			}
		}
	}
	return 0;
}

static int alloc_inode(ext2_fs_instance_t *instance, struct ext2_inode *inode) {
	int i;
	for (i = 0; i < instance->n_groups; i++) {
		if (instance->group_desc_table[i].bg_free_inodes_count) {
			// TODO: decrement bg_free_inodes_count
			int addr_bitmap = instance->group_desc_table[i].bg_inode_bitmap;
			int addr_table = instance->group_desc_table[i].bg_inode_table;
			uint8_t *inode_bitmap = kmalloc(1024 << instance->superblock.s_log_block_size);
	
			instance->read_data(instance->super.device, inode_bitmap, (1024 << instance->superblock.s_log_block_size), addr_bitmap * (1024 << instance->superblock.s_log_block_size));

			uint32_t ib;
			for (ib = 0; ib < instance->superblock.s_inodes_per_group; ib++) {
				int inode_n = i * instance->superblock.s_inodes_per_group + ib + 1;
				if ((inode_bitmap[ib/8] & (1 << (ib % 8))) == 0) {
					instance->write_data(instance->super.device, inode, sizeof(struct ext2_inode), addr_table * (1024 << instance->superblock.s_log_block_size) + sizeof(struct ext2_inode) * ib);
					inode_bitmap[ib/8] |= (1 << (ib % 8));
					instance->write_data(instance->super.device, &(inode_bitmap[ib/8]), sizeof(uint8_t), addr_bitmap * (1024 << instance->superblock.s_log_block_size) + ib / 8);
					return inode_n;
				}
			}
		}
	}
	return 0;
}

static int alloc_block_inode(ext2_fs_instance_t *instance, int inode) {
	struct ext2_inode einode;
	if (read_inode(instance, inode, &einode) >= 0) {
		int i = 0;
		while (einode.i_block[i] > 0 && i < 12) i++;
		if (i < 12) {
			einode.i_block[i] = alloc_block(instance);
			write_inode(instance, inode, &einode);
			return einode.i_block[i];
		}
	}
	return 0;
}


static void free_block(ext2_fs_instance_t *instance, uint32_t blk) {
	int i = blk / instance->superblock.s_blocks_per_group; // Groupe de block.
	int ib = blk - instance->superblock.s_blocks_per_group * i; // Indice dans le groupe.
	int addr_bitmap = instance->group_desc_table[i].bg_block_bitmap;

	uint8_t block_bitmap;
	instance->read_data(instance->super.device, &(block_bitmap), sizeof(uint8_t), addr_bitmap * (1024 << instance->superblock.s_log_block_size) + ib / 8);
	block_bitmap &= ~(1 << (ib % 8));
	instance->write_data(instance->super.device, &(block_bitmap), sizeof(uint8_t), addr_bitmap * (1024 << instance->superblock.s_log_block_size) + ib / 8);
	
	// TODO: increment bg_free_blocks_count
}

//static void free_inode(ext2_fs_instance_t *instance, int inode) {
//	int i = inode / instance->superblock.s_blocks_per_group; // Groupe de block.
//	int ib = inode - instance->superblock.s_blocks_per_group * i; // Indice dans le groupe.
//	int addr_bitmap = instance->group_desc_table[i].bg_inode_bitmap;
//
//	uint8_t block_bitmap;
//	instance->read_data(instance->super.device, &(block_bitmap), sizeof(uint8_t), addr_bitmap * (1024 << instance->superblock.s_log_block_size) + ib / 8);
//	block_bitmap &= ~(1 << (ib % 8));
//	instance->write_data(instance->super.device, &(block_bitmap), sizeof(uint8_t), addr_bitmap * (1024 << instance->superblock.s_log_block_size) + ib / 8);
//}


static void add_dir_entry(ext2_fs_instance_t *instance, int inode, const char *name, int type, int n_inode) {
	struct blk_t *blocks = addr_inode_data(instance, inode);

	if (blocks == NULL) {
		// TODO: create block. Ne devrait pas arriver...
	}

	int addr_debut = blocks->addr;

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
	struct blk_t *blocks = addr_inode_data(instance, inode);

	if (blocks == NULL) {
		return;
	}

	int addr_debut = blocks->addr;

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
		dir2 = dir;
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
	struct blk_t *blocks = addr_inode_data(instance, inode);

	if (blocks == NULL) {
		if (alloc_block_inode(instance, inode) >= 0) {
			blocks = addr_inode_data(instance, inode);
		} else {
			return;
		}
	}
	int addr = blocks->addr;
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

static int ext2_truncate_inode(ext2_fs_instance_t *instance, int inode, off_t off) {
	uint32_t size = off;
	struct ext2_inode einode;
	if (read_inode(instance, inode, &einode) >= 0) {
		struct blk_t *blocks = addr_inode_data(instance, inode);
		struct blk_t *aux = blocks;
		struct blk_t *prec = NULL;
		int first = 1;
		while (aux) {
			if (off <= 0) {
				free_block(instance, aux->addr);
				if (first) {
					if (prec == NULL) {
						blocks = NULL;
					} else {
						prec->next = NULL;
					}
					first = 0;
				}
				prec = aux;
				aux = aux->next;
				kfree(prec);
			} else {
				off -= 1024 << instance->superblock.s_log_block_size;
				prec = aux;
				aux = aux->next;
			}
		}
		while (off > 0) {
			struct blk_t *element = kmalloc(sizeof(struct blk_t));
			element->addr = alloc_block(instance);
			element->next = NULL;
			if (prec == NULL) {
				blocks = element;
			} else {
				prec->next = element;
			}
			prec = element;
			off -= 1024 << instance->superblock.s_log_block_size;
		}
		einode.i_size = size;
		update_blocks(instance, &einode, blocks);
		write_inode(instance, inode, &einode);
		return 0;
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
	inode->i_fs_specific = extra_data;

	return inode;
}
