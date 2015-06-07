/**
 * @file ext2_internal.h
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
 * Description de ce que fait le fichier
 */

#ifndef _EXT2_INTERNAL_H_
#define _EXT2_INTERNAL_H_

#include <fs/devfs.h>
#include <vfs.h>

#define EXT2_ROOT_INO 2 /**< Numéro de l'inode racine. */


/**
 * Structure du Ext2 Super Block
 * (Repris du code de Linux)
 * http://www.nongnu.org/ext2-doc/ext2.html#SUPERBLOCK
 */
struct ext2_super_block {
	uint32_t  s_inodes_count;		/**< Inodes count */
	uint32_t  s_blocks_count;		/**< Blocks count */
	uint32_t  s_r_blocks_count;		/**< Reserved blocks count */
	uint32_t  s_free_blocks_count;  /**< Free blocks count */
	uint32_t  s_free_inodes_count;  /**< Free inodes count */
	uint32_t  s_first_data_block;	/**< First Data Block */
	uint32_t  s_log_block_size;		/**< Block size */
	uint32_t  s_log_frag_size;		/**< Fragment size */
	uint32_t  s_blocks_per_group;	/**< # Blocks per group */
	uint32_t  s_frags_per_group;	/**< # Fragments per group */
	uint32_t  s_inodes_per_group;	/**< # Inodes per group */
	uint32_t  s_mtime;				/**< Mount time */
	uint32_t  s_wtime;				/**< Write time */
	uint16_t  s_mnt_count;			/**< Mount count */
	uint16_t  s_max_mnt_count;		/**< Maximal mount count */
	uint16_t  s_magic;				/**< Magic signature */
	uint16_t  s_state;				/**< File system state */
	uint16_t  s_errors;				/**< Behaviour when detecting errors */
	uint16_t  s_minor_rev_level;	/**< minor revision level */
	uint32_t  s_lastcheck;			/**< time of last check */
	uint32_t  s_checkinterval;		/**< max. time between checks */
	uint32_t  s_creator_os;			/**< OS qui a formaté */
	uint32_t  s_rev_level;			/**< Revision level */
	uint16_t  s_def_resuid;			/**< Default uid for reserved blocks */
	uint16_t  s_def_resgid;			/**< Default gid for reserved blocks */
// -- EXT2_DYNAMIC_REV Specific --
	uint32_t  s_first_ino;			/**< First non-reserved inode */
	uint16_t  s_inode_size;			/**< size of inode structure */
	uint16_t  s_block_group_nr;		/**< block group # of this superblock */
	uint32_t  s_feature_compat;		/**< compatible feature set */
	uint32_t  s_feature_incompat;   /**< incompatible feature set */
	uint32_t  s_feature_ro_compat;  /**< readonly-compatible feature set */
	uint8_t   s_uuid[16];			/**< 128-bit uuid for volume */
	char      s_volume_name[16];	/**< volume name */
	char      s_last_mounted[64];   /**< directory where last mounted */
	uint32_t  s_algorithm_usage_bitmap; /**< For compression */
// -- Performance Hints --
	uint8_t   s_prealloc_blocks;	/**< Nr of blocks to try to preallocate*/
	uint8_t   s_prealloc_dir_blocks;  /**< Nr to preallocate for dirs */
	uint16_t  s_padding1;			/**< unused. */
// -- Journaling Support --
	uint8_t   s_journal_uuid[16];	/**< uuid of journal superblock */
	uint32_t  s_journal_inum;		/**< inode number of journal file */
	uint32_t  s_journal_dev;		/**< device number of journal file */
	uint32_t  s_last_orphan;		/**< start of list of inodes to delete */
// -- Directory Indexing Support --
	uint32_t  s_hash_seed[4];		/**< HTREE hash seed */
	uint8_t   s_def_hash_version;	/**< Default hash version to use */
	uint8_t   s_padding[3];		/**< unused. */
// -- Other options --
	uint32_t  s_default_mount_opts; /**< Option de mount par défaut. */
	uint32_t  s_first_meta_bg;		/**< First metablock block group */
	uint8_t   s_reserved[760];		/**< Padding to the end of the block */
};

/*
 * Codes for operating systems
 */
#define EXT2_OS_LINUX		0 /**< Code pour Linux */
#define EXT2_OS_HURD		1 /**< Code pour Hurd */
#define EXT2_OS_MASIX		2 /**< Code pour Masix */
#define EXT2_OS_FREEBSD		3 /**< Code pour Freebsd */
#define EXT2_OS_LITES		4 /**< Code pour Lites */

/*
 * Revision levels
 */
#define EXT2_GOOD_OLD_REV 0		/**< The good old (original) format */
#define EXT2_DYNAMIC_REV  1		/**< V2 format w/ dynamic inode sizes */

/**
 * Groupe.
 */
struct ext2_group_desc {
  uint32_t  bg_block_bitmap;		/**< Blocks bitmap block */
  uint32_t  bg_inode_bitmap;		/**< Inodes bitmap block */
  uint32_t  bg_inode_table;			/**< Inodes table block */
  uint16_t  bg_free_blocks_count;	/**< Free blocks count */
  uint16_t  bg_free_inodes_count;	/**< Free inodes count */
  uint16_t  bg_used_dirs_count;		/**< Directories count */
  uint16_t  bg_pad;					/**< unused. */
  uint32_t  bg_reserved[3];			/**< reserved. */
};

/**
 * Copie mémoire des infos du disque pour limiter le nombre d'accès inutiles.
 */
struct ext2_group_desc_internal {
	uint8_t *inode_bitmap; /**< Inode bitmap. */
	struct ext2_inode *inodes; /**< Tableau d'inodes. */
	//TODO: Je pourrais faire de même avec les blocks. À voir...
};

/**
 * Structure of an inode on the disk
 */
struct ext2_inode {
	uint16_t	i_mode;		/**< File mode */
	uint16_t	i_uid;		/**< Low 16 bits of Owner Uid */
	uint32_t	i_size;		/**< Size in bytes */
	uint32_t	i_atime;	/**< Access time */
	uint32_t	i_ctime;	/**< Creation time */
	uint32_t	i_mtime;	/**< Modification time */
	uint32_t	i_dtime;	/**< Deletion Time */
	uint16_t	i_gid;		/**< Low 16 bits of Group Id */
	uint16_t	i_links_count;	/**< Links count */
	uint32_t	i_blocks;	/**< Blocks count */
	uint32_t	i_flags;	/**< File flags */
	uint32_t	i_osd1;		/**< OS dependent 1 */
	uint32_t	i_block[15];	/**< Pointers to blocks */
	uint32_t	i_generation;	/**< File version (for NFS) */
	uint32_t	i_file_acl;	/**< File ACL */
	uint32_t	i_dir_acl;	/**< Directory ACL */
	uint32_t	i_faddr;	/**< Fragment address */
	uint32_t 	i_osd2[3];	/**< OS dependent 2 */
};

// Types d'inode :
#define EXT2_FT_UNKNOWN		0 /**< Filetype inconu. */
#define EXT2_FT_REG_FILE	1 /**< Fichier régulier. */
#define EXT2_FT_DIR			2 /**< Dossier. */
#define EXT2_FT_CHRDEV		3 /**< Char device. */
#define EXT2_FT_BLKDEV		4 /**< Block device. */
#define EXT2_FT_FIFO		5 /**< Fifo special file. */
#define EXT2_FT_SOCK		6 /**< Socket special file. */
#define EXT2_FT_SYMLINK		7 /**< Lien symbolique. */

/**
 * @brief Structure qui représente une entrée dans un dossier ext2. Format
 * imposé par Ext2.
 */
struct ext2_directory {
	uint32_t	inode;      /**< Numero d'inode. */
	uint16_t	rec_len;    /**< Taille occupée par cette entrée. */
	uint8_t		name_len;   /**< Longueur du nom de fichier. */
	uint8_t		file_type;  /**< Type de fichier. */
	char		name[256];  /**< Nom du fichier. */
};

/**
 * @brief Structure chaînée pour enregistrer les entrées d'un dossier.
 */
struct directories_t {
	struct ext2_directory *dir; /**< Pointeur vers une entrée de dossier ext2. */
	struct directories_t *next; /**< Pointeur vers la prochaine structure du même type. */
};

// -- file format --
#define EXT2_S_IFSOCK	0xC000	/**< socket */
#define EXT2_S_IFLNK	0xA000	/**< symbolic link */
#define EXT2_S_IFREG	0x8000	/**< regular file */
#define EXT2_S_IFBLK	0x6000	/**< block device */
#define EXT2_S_IFDIR	0x4000	/**< directory */
#define EXT2_S_IFCHR	0x2000	/**< character device */
#define EXT2_S_IFIFO	0x1000	/**< fifo */
// -- process execution user/group override --
#define EXT2_S_ISUID	0x0800	/**< Set process User ID */
#define EXT2_S_ISGID	0x0400	/**< Set process Group ID */
#define EXT2_S_ISVTX	0x0200	/**< sticky bit */
// -- access rights --
#define EXT2_S_IRUSR	0x0100	/**< user read */
#define EXT2_S_IWUSR	0x0080	/**< user write */
#define EXT2_S_IXUSR	0x0040	/**< user execute */
#define EXT2_S_IRGRP	0x0020	/**< group read */
#define EXT2_S_IWGRP	0x0010	/**< group write */
#define EXT2_S_IXGRP	0x0008	/**< group execute */
#define EXT2_S_IROTH	0x0004	/**< others read */
#define EXT2_S_IWOTH	0x0002	/**< others write */
#define EXT2_S_IXOTH	0x0001	/**< others execute */

/**
 * @brief Instance de FS Ext2.
 */
typedef struct _ext2_fs_instance_t {
	fs_instance_t super; /**< Common fields for all FS instances. */
	dentry_t * root; /**< Inode root. */
	struct ext2_super_block superblock; /**< Superblock. */
	struct ext2_group_desc *group_desc_table; /**< Block group descriptor table. */
	int n_groups;   /**< Number of entries in the group desc table. */
	blkdev_read_t read_data; /**< Function to read data. */
	blkdev_write_t write_data; /**< Function to write data. */
	struct ext2_group_desc_internal *group_desc_table_internal; /**< Copy of inodes (only, for now?) */
} ext2_fs_instance_t;

/**
 * @brief Unmount d'un point de montage.
 *
 * @param instance L'instance de fs que l'on veut démonter.
 */
void umount_EXT2(fs_instance_t *instance);

/**
 * @brief Lecture d'un fichier.
 *
 * @param ofd Descripteur du fichier ouvert.
 * @param buf Buffer où stocker les octets lus.
 * @param size Nombre d'octets à lire.
 *
 * @return nombre d'octets réellement lus.
 */
ssize_t ext2_read(open_file_descriptor * ofd, void * buf, size_t size);

/**
 * @brief Ecriture d'un fichier.
 *
 * @param ofd Descripteur du fichier ouvert.
 * @param buf Buffer contenant les octets à écrire.
 * @param size Nombre d'octets à écrire.
 *
 * @return nombre d'octets réellement écrits.
 */
ssize_t ext2_write (open_file_descriptor * ofd, const void *buf, size_t size);

/**
 * @brief Fermeture d'un fichier ouvert.
 *
 * @param ofd Descripteur du fichier ouvert.
 *
 * @return 0 en cas de succès, -1 sinon.
 */
int ext2_close(open_file_descriptor *ofd);

/**
 * @brief Lecture du contenu d'un dossier.
 *
 * @param ofd Descripteur de fichier qui correspond au dossier.
 * @param entries Buffer qui va recevoir les entrées de dossier.
 * @param size Nombre d'octets dans le buffer au maximum.
 *
 * @return Nombre d'octets écrits dans entries.
 */
int ext2_readdir(open_file_descriptor * ofd, char * entries, size_t size);

/**
 * @brief Récupère l'état d'un fichier ou dossier.
 *
 * @param instance Instance de fs.
 * @param path Chemin du fichier.
 * @param stbuf Endroit où enregistrer les infos.
 *
 * @return 0 en cas de succès.
 */
int ext2_stat(fs_instance_t *instance, const char *path, struct stat *stbuf);

/**
 * @brief Création d'un noeud (fichier, dossier, fichier spécial...)
 *
 * @param dir Inode du dossier parent. 
 * @param dentry Structure contenant le nom et un pointeur vers l'inode qui sera créé.
 * @param mode Droits sur le noeud.
 * @param dev En cas de fichier spécial (device)
 *
 * @return 0 en cas de succès.
 */
int ext2_mknod(inode_t *dir, dentry_t *dentry, mode_t mode, dev_t dev);


/**
 * @brief Création d'un dossier.
 *
 * @param dir Inode du dossier parent. 
 * @param dentry Structure contenant le nom et un pointeur vers l'inode qui sera créé.
 * @param mode Droits sur le dossier.
 *
 * @return 0 en cas de succès.
 */
int ext2_mkdir(inode_t *dir, dentry_t *dentry, mode_t mode);

/**
 * @brief Suppression d'un noeud.
 *
 * @param dir Inode du dossier parent. 
 * @param dentry Entrée à supprimer.
 *
 * @return 0 en cas de succès.
 */
int ext2_unlink(inode_t *dir, dentry_t *dentry);

/**
 * @brief Change la taille d'un fichier.
 *
 * @param inode Inode à modifier.
 * @param off Nouvelle taille.
 *
 * @return 0 en cas de succès.
 */
int ext2_truncate(inode_t *inode, off_t off);

/**
 * @brief Suppression d'un dossier vide.
 *
 * @param dir Inode du dossier parent. 
 * @param dentry Dossier à supprimer.
 *
 * @return 0 en cas de succès.
 */
int ext2_rmdir(inode_t *dir, dentry_t *dentry);

/**
 * Change les attributs d'un inode.
 *
 * @param inode Inode à modifier.
 * @param attr Nouveaux attributs.
 *
 * @return 0 en cas de succès.
 */
int ext2_setattr(inode_t *inode, file_attributes_t *attr);

/**
 * Retourne le noeud racine.
 *
 * @param instance Instance de FS.
 *
 * @return le noeud racine.
 */
dentry_t *ext2_getroot(struct _fs_instance_t *instance);

/**
 * Recherche le directory entry qui correspond au nom donné.
 *
 * @param instance Instance de FS.
 * @param dentry Parent directory entry.
 * @param name Nom de l'entrée.
 *
 * @return Directory Entry de l'entrée recherchée ou NULL.
 */
dentry_t* ext2_lookup(struct _fs_instance_t *instance, struct _dentry_t* dentry, const char * name);

/**
 * @brief Renomme ou déplace un fichier.
 *
 * @param old_dir Inode du dossier parent de l'ancien nom.
 * @param old_dentry Nom et inode à déplacer.
 * @param new_dir Inode du dossier de destination.
 * @param new_dentry Nom et inode de destination.
 *
 * @return 0 en cas de succès.
 */
int ext2_rename(inode_t *old_dir, dentry_t *old_dentry, inode_t *new_dir, dentry_t *new_dentry);


int ext2_symlink(inode_t *dir, dentry_t *dentry, const char* target);


dentry_t * init_rootext2fs(ext2_fs_instance_t *instance);

extern struct _open_file_operations_t ext2fs_fops; /**< Pointeurs sur les fonctions de manipulation de fichiers pour ce FS. */

#endif
