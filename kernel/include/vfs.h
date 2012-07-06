/**
 * @file vfs.h
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
 * @brief Virtual File System
 */

#ifndef _VFS_H_
#define _VFS_H_

#include <types.h>
#include <fd_types.h>
#include <kstat.h>

struct _fs_instance_t;
struct _dentry_t;
struct _inode_t;
struct _file_attributes_t;


/**
 * @brief Structure qui représente un FS.
 */
typedef struct {
	char *name;	/**< Nom du FS, c'est aussi son "type" utilisé lors du mount. */
	//XXX: ajouter device en argument de mount.
	struct _fs_instance_t * (*mount) (open_file_descriptor*); /**< Pointeur vers la fonction mount du FS. */
	void (*umount) (struct _fs_instance_t *); /**< Pointeur vers la fonction umount du FS. */
} file_system_t;

/**
 * @brief Instance d'un couple FS/Device monté.
 */
typedef struct _fs_instance_t {
	file_system_t *fs;							/**< Pointeur vers le FS utilisé. */ //XXX: Est-ce utile ?
	open_file_descriptor * device;																											/**< Device utilisé. */
	struct _dentry_t* (*getroot) (struct _fs_instance_t *);
	struct _dentry_t* (*lookup) (struct _fs_instance_t *, struct _dentry_t*, const char *);
	open_file_descriptor * (*open) (struct _fs_instance_t *, const char * , uint32_t);	/**< Fonction pour ouvrir un fichier. */
	int (*mkdir) (struct _inode_t *, struct _dentry_t *, mode_t);											/**< Création d'un dossier. */
	int (*mknod) (struct _inode_t *, struct _dentry_t *, mode_t, dev_t);								/**< Création d'un noeud. */
	int (*stat) (struct _fs_instance_t *, const char *, struct stat *);									/**< Obtenir le status d'un noeud. */
	int (*unlink) (struct _inode_t *, struct _dentry_t *);															/**< Suppression d'un noeud. */
	int (*rmdir) (struct _inode_t *, struct _dentry_t *);																/**< Suppression d'un dossier vide. */
	int (*truncate) (struct _inode_t *, off_t size);	/**< Changer la taille d'un fichier. */
	int (*setattr) (struct _inode_t *inode, struct _file_attributes_t *attr);
	int (*rename) (struct _inode_t *old_dir, struct _dentry_t *old_dentry, struct _inode_t *new_dir, struct _dentry_t *new_dentry);
} fs_instance_t;

/**
 * Cellule de la liste des points de montage.
 */
typedef struct _mounted_fs_t {
	fs_instance_t *instance;
	char *name;
	struct _mounted_fs_t *next; /**< Prochaine cellule. */
} mounted_fs_t;

typedef struct _inode_t {
	unsigned long i_ino; /**< Inode number */
	int  i_mode;   /**< File mode */
	uid_t  i_uid;    /**< Low 16 bits of Owner Uid */
	gid_t  i_gid;    /**< Low 16 bits of Group Id */
	off_t  i_size;   /**< Size in bytes */
	time_t  i_atime;  /**< Access time */
	time_t  i_ctime;  /**< Creation time */
	time_t  i_mtime;  /**< Modification time */
	time_t  i_dtime;  /**< Deletion Time */
	unsigned char  i_nlink;  /**< Links count */
	unsigned char  i_flags;  /**< File flags */
	unsigned long  i_blocks; /**< Blocks count */
	fs_instance_t *i_instance; /**< Instance de fs auquel appartient cet inoeud. */
//	inode_ops_t *i_ops; /**< Inode operations. */
	struct _open_file_operations_t *i_fops; /**< Ofd operations. */
	void *i_fs_specific; /**< Extra data */
} inode_t;

typedef struct _dentry_t {
	const char *d_name;
	inode_t *d_inode;
} dentry_t;

struct nameidata {
	int flags;
	dentry_t *dentry;
	mounted_fs_t *mnt;
	const char *last;
};

#define ATTR_UID 1 /**< Attribut uid valide. */
#define ATTR_GID (1 << 1) /**< Attribut gid valide. */
#define ATTR_MODE (1 << 2) /**< Attribut mode valide. */
#define ATTR_ATIME (1 << 3) /**< Attribut atime valide. */
#define ATTR_MTIME (1 << 4) /**< Attribut mtime valide. */
#define ATTR_CTIME (1 << 5) /**< Attribut ctime valide. */

typedef struct _file_attributes_t {
	int mask; /**< Champs valides. */
	struct stat stbuf; /**< Structure contenant les informations. */
} file_attributes_t;

/**
 * @brief Enregistrer un FS dans le VFS pour le rendre disponible.
 */
void vfs_register_fs(file_system_t *fs);

/**
 * @brief Ouverture d'un fichier.
 *
 * @param pathname Chemin du fichier à ouvrir.
 * @param flags Mode d'ouverture.
 *
 * @return Un descripteur de fichier ouvert.
 */
open_file_descriptor* vfs_open(const char * pathname, uint32_t flags);

/**
 * @brief Montage d'un device sur un certain point de montage.
 *
 * @param device à monter.
 * @param mountpoint nom du dossier.
 * @param type FS à utiliser.
 */
void vfs_mount(const char *device, const char *mountpoint, const char *type);

/**
 * @brief Démonte un point de montage.
 *
 * @return 0 en cas de succès.
 */
int vfs_umount(const char *mountpoint);

/**
 * @brief Création d'un dossier.
 *
 * @param pathname Chemin du dossier.
 * @param mode Droits sur le dossier.
 *
 * @return 0 en cas de succès.
 */
int vfs_mkdir(const char * pathname, mode_t mode);

/**
 * @brief Obtient des infos sur un noeud.
 *
 * @param pathname Chemin du fichier.
 * @param
 *
 * @return 0 en cas de succès.
 */
int vfs_stat(const char *pathname, struct stat *);

/**
 * @brief Suppression d'un noeud.
 *
 * @param pathname Chemin du fichier.
 *
 * @return 0 en cas de succès.
 */
int vfs_unlink(const char *pathname);

/**
 * @brief Création d'un nouveau noeud.
 *
 * @param path Chemin du fichier.
 * @param mode Droits et type de fichier.
 * @param dev Si c'est un fichier spécial, le device associé.
 *
 * @return 0 en cas de succès.
 */
int vfs_mknod(const char * path, mode_t mode, dev_t dev);


int vfs_chmod(const char *pathname, mode_t mode);
int vfs_chown(const char *pathname, uid_t owner, gid_t group);
int vfs_utimes(const char *pathname, const struct timeval tv[2]);
int vfs_rename(const char *oldpath, const char *newpath);

/**
 * @brief Suppression d'un dossier.
 *
 * @param pathname Chemin du dossier.
 *
 * @return 0 en cas de succès.
 */
int vfs_rmdir(const char *pathname);

/**
 * @brief Lecture de plusieurs entrées d'un dossier.
 *
 * @param ofd du dossier ouvert.
 * @param entries Contenu du dossier, encodé dans une chaîne de caractères.
 * @param size Taille de la chaîne entries.
 *
 * @return 0 en cas de succès.
 */
int vfs_readdir(open_file_descriptor * ofd, char * entries, size_t size);

/**
 * @brief Fermeture d'un fichier ouvert.
 *
 * @param ofd du fichier ouvert.
 * 
 * @return 0 en cas de succès.
 */
int vfs_close(open_file_descriptor *ofd);

void vfs_init();

#endif
