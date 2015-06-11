/**
 * @file fat_internal.h
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

#ifndef _FAT_INTERNAL_H_
#define _FAT_INTERNAL_H_

#include <types.h>
#include <clock.h>
#include <vfs.h>
#include <fs/devfs.h>

// FAT low level.

/**
 *  Boot Sector
 */
typedef struct _fat_BS {
	uint8_t   bootjmp[3];         //0x00
	uint8_t   oem_name[8]; /**< Nom de l'OS qui a formaté le volume. */ //0x03
	uint16_t  bytes_per_sector; /**< Nombre d'octets par secteur. */    //0x0b
	uint8_t   sectors_per_cluster; /**< Nombre de secteurs par cluster. */  //0x0d
	uint16_t  reserved_sector_count; /**< Nombre de secteurs réservés. */   //0x0e
	uint8_t   table_count; /**< Nombre de FATs. */  //0x10
	uint16_t  root_entry_count; /**< Nombre d'entrées dans le dossier racine. */ //0x11
	uint16_t  total_sectors_16; /**< Nombre de secteurs, si 0 alors utilise total_sectors_32. */     //0x13
	uint8_t   media_type; /**< Type de disque. */        //0x15
	uint16_t  table_size_16; /**< Nombre de secteurs pour une FAT (FAT 12 et 16 uniquement). */        //0x16
	uint16_t  sectors_per_track; /**< Secteurs par piste. */      //0x18
	uint16_t  head_side_count; /**< Nombre de secteurs par tête. */     //0x1a
	uint32_t  hidden_sector_count; /**< Le nombre de secteurs entre le début et la table de partition. */    //0x1c
	uint32_t  total_sectors_32; /**< Nombre de secteurs (si > 2^16) */    //0x20
} __attribute__ ((packed)) fat_BS_t; 

/**
 *  Extended BIOS Parameter Block (FAT12 / FAT16)
 */
typedef struct _fat_extended_BIOS_16 {
	uint8_t   bios_drive_num;       //0x24
	uint8_t   reserved; /**< Reserved */        //0x25
	uint8_t   ext_boot_signature; /**< Ext Boot record signature = 29h */       //0x26
	uint32_t  volume_id; /**< Identifiant du volume (serial). */         //0x27
	uint8_t   volume_label[11]; /**< Nom du volume. */     //0x2b
	uint8_t   fat_type_label[8]; /**< Nom du FS (FAT12, FAT16 ou FAT32). */  //0x36
	uint8_t   os_boot_code[448];      //0x3e
	uint16_t  boot_sector_sign;     //0x1fe
} __attribute__ ((packed)) fat_extended_BIOS_16_t;

/**
 *  Extended BIOS Parameter Block (FAT 32)
 */
typedef struct _fat_extended_BIOS_32 {
	uint32_t  table_size_32;       //0x24
	uint16_t  fat_flags;           //0x28
	uint16_t  version;             //0x2a
	uint32_t  cluster_root_dir;    //0x2c
	uint16_t  sector_fs_info;      //0x30
	uint16_t  sector_bs_backup;    //0x32
	uint8_t   reserved[12];        //0x34
	uint8_t   bios_drive_num;      //0x40
	uint8_t   reserved2;           //0x41
	uint8_t   ext_boot_signature;  //0x42
	uint32_t  volume_id;           //0x43
	uint8_t   volume_label[11];    //0x47
	uint8_t   fat_type_label[8];   //0x52
	uint8_t   os_boot_code[420];   //0x5a
	uint16_t  boot_sector_sign;    //0x1fe
} __attribute__ ((packed)) fat_extended_BIOS_32_t;


/**
 * Représentation du temps au format FAT.
 */
typedef struct _fat_time {
	unsigned int seconds2 : 5; /**< Nombre de secondes. */
	unsigned int minutes : 6;  /**< Nombre de minutes. */
	unsigned int hours : 5;    /**< Nombre d'heures. */
} __attribute__ ((packed)) fat_time_t;

/**
 * Représentation de la date au format FAT.
 */
typedef struct _fat_date {
	unsigned int day : 5; /**< Jour. */
	unsigned int month : 4; /**< Mois. */
	unsigned int year : 7; /**< Année. */
} __attribute__ ((packed)) fat_date_t;

/**
 * Entrée de dossier.
 */
typedef struct _fat_dir_entry {
	char      utf8_short_name[8]; /**< Nom court. */
	char      file_extension[3];  /**< Extension. */
	uint8_t   file_attributes;    /**< Attributs du fichier (write, hidden, etc.)*/
	uint8_t   reserved;						/**< réservé. */
	uint8_t   create_time_ms;			/**< Si existant : create time [0 - 199] x 10ms
Sinon : premier caractère (puisqu'on le remplace). */
	fat_time_t  create_time;		/**< Heure de création. */
	fat_date_t  create_date;		/**< Date de création. */
	fat_date_t  last_access_date; /**< Date dernier accès. */
	uint16_t  ea_index;
	fat_time_t  last_modif_time; /**< Heure de dernière modification. */
	fat_date_t  last_modif_date; /**< Date de dernière modification. */
	uint16_t  cluster_pointer;	/**< Cluster où se situe le fichier. */
	uint32_t  file_size;				/**< Taille du fichier. */

}__attribute__((packed)) fat_dir_entry_t;

/**
 * Entrée pour les noms de fichier long.
 */
typedef struct {
	uint8_t   seq_number; /**< Numéro de séquence. */
	uint8_t   filename1[10]; /**< Partie du nom de fichier. */
	uint8_t   attributes; /**< Attributs (Toujours 0x0F). */
	uint8_t   reserved; /**< always 0x0. */
	uint8_t   checksum; /**< checksum. */
	uint8_t   filename2[12]; /**< Suite du nom de fichier. */
	uint16_t  cluster_pointer; /**< always 0x000. */
	uint8_t   filename3[4]; /**< Suite du nom de fichier. */
}__attribute__((packed)) lfn_entry_t;


// FAT higher level.

/**
 * Liste chaînée pour stocker les entrées d'un dossier.
 */
typedef struct _directory_entry {
	char name[256]; /**< Nom de l'entrée. */
	uint8_t attributes; /**< Attributs du fichier/dossier. */
	uint32_t size; /**< Taille. */
	time_t access_time; /**< Date du dernier accès. */
	time_t modification_time; /**< Date de la dernière modification. */
	time_t creation_time; /**< Date de création. */
	uint32_t cluster; /**< Cluster où est le fichier/dossier. */
	struct _directory_entry *next; /**< Pointeur vers la prochaine entrée du dossier ou NULL. */
} directory_entry_t;

/**
 * Structure de haut niveau pour stocker les informations d'un dossier.
 */
typedef struct _directory {
	directory_entry_t *entries; /**< Liste des entrées dans ce dossier. */
	int total_entries; /**< Nombre d'entrées dans le dossier. */
	char name[256]; /**< Nom du dossier. */
	uint32_t cluster; /**< Cluster où est enregistré le dossier. */
} directory_t;

/**
 * Types de FAT.
 */
typedef enum {
	FAT12,
	FAT16,
	FAT32
} fat_t;

/**
 *  Structure qui contient tout ce qui est utilisé par le driver FAT.
 */
typedef struct _fat_info {
	fat_BS_t BS; /**< Secteur de Boot. */
	fat_extended_BIOS_16_t *ext_BIOS_16; /**< Infos supplémentaires pour FAT16. */
	fat_extended_BIOS_32_t *ext_BIOS_32; /**< Infos supplémentaires pour FAT32. */
	unsigned int *addr_fat; /**< Adresses des FAT. */
	unsigned int addr_root_dir; /**< Adresse du dossier racine (attention spécificité FAT32). */
	unsigned int addr_data; /**< Adresse des premiers clusters de données. */
	unsigned int *file_alloc_table; /**< FAT en mémoire. */
	unsigned int total_data_clusters; /**< Nombre de clusters de données. */
	unsigned int table_size; /**< Taille de la FAT. */
	fat_t fat_type; /**< Type de FAT (12, 16 ou 32) */
	unsigned int bytes_per_cluster; /**< Nombre d'octets dans un cluster. */
} fat_info_t;

/**
 * Instance de FS de type FAT.
 */
typedef struct _fat_fs_instance_t {
	fs_instance_t super; /**< Classe parente (fs_instance_t). */
	dentry_t * root;
	fat_info_t fat_info; /**< Informations sur le volume monté. */
	blkdev_read_t read_data; /**< Fonction pour lire une donnée. */
	blkdev_write_t write_data; /**< Fonction pour écriture une donnée. */
} fat_fs_instance_t;

/**
 * @brief Données supplémentaires qui sont ajoutés à l'ofd lors du open.
 */
typedef struct _fat_extra_data_t {
	int first_cluster; /**< Adresse du premier cluster. */
	int current_cluster; /**< Cluster courant. */
	unsigned int current_octet_buf; /**< Position dans le buffer. */
	uint8_t buffer[512]; /**< Buffer pour limiter les appels au disque. */
} fat_extra_data_t;

/**
 * @brief Structure fille d'un dentry_t qui ajoute quelques données.
 */
typedef struct _fat_direntry_t {
	dentry_t super;
	directory_entry_t *fat_entry;
	directory_t *fat_directory;
} fat_direntry_t;

extern struct _open_file_operations_t fatfs_fops; /**< Pointeurs sur les fonctions de manipulation de fichiers pour ce FS. */

/**
 * Fonction qui permet de récupérer le dentry à la racine.
 *
 * @param instance Instance de FS.
 *
 * @return Dossier racine.
 */
dentry_t *fat_getroot(struct _fs_instance_t *instance);

/**
 * Fonction de lookup : retourne l'entrée de dossier qui correspond au nom demandé.
 *
 * @param instance Instance de FS.
 * @param dentry Dossier parent.
 * @param name Nom de l'entrée dans ce dossier.
 *
 * @return Entrée de dossier correspondant.
 */
dentry_t* fat_lookup(struct _fs_instance_t *instance, struct _dentry_t* dentry, const char * name);

/**
 * Ouvre le répertoire racine.
 *
 * @param instance Instance de FS.
 *
 * @return Dossier racine.
 */
directory_t * open_root_dir(fat_fs_instance_t *instance);

/**
 * Lecture de la FAT.
 *
 * @param instance Instance de FS.
 */
void read_fat(fat_fs_instance_t *instance);


/**
 * Démonte un FS.
 * 
 * @param instance Instance de FS.
 */
void umount_FAT(fs_instance_t *instance);

/**
 * Lecture d'un dossier.
 *
 * @param ofd Descripteur de fichier qui correspond au dossier.
 * @param entries Buffer qui va recevoir les entrées de dossier.
 * @param size Nombre d'octets dans le buffer au maximum.
 *
 * @return Nombre d'octets écrits dans entries.
 */
int fat_readdir(open_file_descriptor * ofd, char * entries, size_t size);

/**
 * Création d'un dossier.
 *
 * @param instance Instance de fs.
 * @param path Chemin du dossier à créer.
 * @param mode Droits sur le dossier.
 *
 * @return 0 en cas de succès.
 */
int fat_mkdir(fs_instance_t *instance, const char * path, mode_t mode);

/**
 * Informations sur un fichier.
 *
 * @param instance Instance de fs.
 * @param path Chemin du fichier.
 * @param stbuf Structure pour enregistrer les informations.
 *
 * @return 0 en cas de succès.
 */
int fat_stat(fs_instance_t *instance, const char *path, struct stat *stbuf);

/**
 * Suppression d'un fichier ou dossier (vide).
 *
 * @param instance Instance de fs.
 * @param path Chemin du noeud.
 *
 * @return 0 en cas de succès.
 */
int fat_unlink(fs_instance_t *instance, const char * path);

/**
 * Lecture d'un fichier.
 *
 * @param ofd Descripteur du fichier ouvert.
 * @param buf Buffer où stocker les octets lus.
 * @param count Nombre d'octets à lire.
 *
 * @return nombre d'octets réellement lus.
 */
ssize_t fat_read_file (open_file_descriptor * ofd, void * buf, size_t count);

/**
 * Déplacement dans un fichier.
 *
 * @param ofd Descripteur de fichier ouvert.
 * @param offset Décalage
 * @param whence depuis le debut, la fin ou en relatif.
 *
 * @return 0 en cas de succès, -1 sinon.
 */
int fat_seek_file (open_file_descriptor * ofd, long offset, int whence);

/**
 * Ouverture d'un fichier. Initialise certaines données de l'ofd.
 *
 * @param ofd Descripteur de fichier ouvert.
 *
 * @return 0 en cas de succès.
 */
int fat_open(open_file_descriptor *ofd);

#endif
