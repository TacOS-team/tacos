/**
 * @file elf.h
 *
 * @author TacOS developers 
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
 * @brief Bibliotèque destinée à manipuler les fichiers exécutables ELF
 */

#ifndef _ELF_H_
#define _ELF_H_

typedef unsigned long int 	Elf32_Addr;
typedef unsigned short int 	Elf32_Half;
typedef unsigned long int 	Elf32_Off;
typedef signed long int 	Elf32_Sword;
typedef	unsigned long int	Elf32_Word;


#define EI_NIDENT	16

/**
 * ELF HEADER
 */
typedef struct {
	unsigned char	e_ident[EI_NIDENT];	/**< ELF Identification */
	Elf32_Half		e_type;				/**< Type de fichier ELF */
	Elf32_Half		e_machine;			/**< Architecture de la machine */
	Elf32_Word		e_version;			/**< Version du fichier objet */
	Elf32_Addr		e_entry;			/**< Adresse virtuelle du point d'entrée */
	Elf32_Off		e_phoff;			/**< Offset du program header dans le fichier (0 si aucun program header)*/
	Elf32_Off		e_shoff;			/**< Offset du section header dans le fichier (0 si aucun section header)*/
	Elf32_Word		e_flags;			/**< Flags spécifiques à la machine */
	Elf32_Half		e_ehsize;			/**< Taille du header ELF */
	Elf32_Half		e_phentsize;		/**< Taille d'une entrée dans la table des program header (toutes les entrées ont la même taille) */
	Elf32_Half		e_phnum;			/**< Nombre d'entrées dans la table des program header */
	Elf32_Half		e_shentsize;		/**< Taille d'une entrée dans la table des section header */
	Elf32_Half		e_shnum;			/**< Nombre d'entrées dans la table des section header */
	Elf32_Half		e_shstrndx;			/**< Indice de l'entrée de la table des section header correspondant à la table des strings, si aucune entrée de ce genre, SHN_UNDEF */
} Elf32_Ehdr;

/* Indices pour e_ident: */
#define EI_MAG0		0	/* 0x7F */
#define EI_MAG1		1	/* 'E' */
#define EI_MAG2		2	/* 'L' */
#define EI_MAG3		3	/* 'F' */
#define EI_CLASS	4
	#define ELFCLASSNONE	0	/* Classe invalide */
	#define ELFCLASS32		1	/* Objet 32-bits */
	#define ELFCLASS64		2	/* Objet 64-bits */
	
#define EI_DATA		5
	#define ELFDATANONE		0	/* Encodage des données invalide */
	#define ELFDATA2LSB		1	/* Big-endian */
	#define ELFDATA2MSB		2	/* Little-endian */
	
#define EI_VERSION	6
#define EI_PAD		7


/* Valeurs pour e_type: */
#define ET_NONE		0	/* No type */
#define ET_REL		1	/* Relocatable file */
#define ET_EXE		2	/* Executable file */
#define ET_DYN		3	/* Shared object file */
#define ET_CORE		4	/* Core file */
#define ET_LOPROC	0xff00	/* Processor-specific */
#define ET_HIPROC	0xffff	/* Processor-specific */

/* Valeurs pour e_machine: */
#define EM_NONE		0	/* Aucune architecture spécifiée */
#define EM_M32		1	/* AT&T WE 32100 */
#define EM_SPARC	2	/* SPARC */
#define EM_386		3	/* Intel 80386 */
#define EM_68K		4	/* Motorola 68000 */
#define EM_88K		5	/* Motorola 88000 */
#define EM_860		7	/* Intel 80860 */
#define EM_MIPS		8	/* MIPS RS3000 */

/* Valeurs pour e_version: */
#define EV_NONE		0
#define EV_CURRENT	1

/*******************************
 * 
 * PROGRAM HEADER
 * 
 *******************************/
 
/* Indices des sections spéciales */
#define	SHN_UNDEF		0
#define	SHN_LORESERVE	0xff00
#define	SHN_LOPROC		0xff00
#define	SHN_HIPROC		0xff1f
#define	SHN_ABS			0xfff1
#define	SHN_COMMON		0xfff2
#define	SHN_HIRESERVE	0xffff

typedef struct{
	Elf32_Word	p_type;		/* Type de segment */
	Elf32_Off	p_offset;	/* Offset du segment dans le fichier */
	Elf32_Addr	p_vaddr;	/* Adresse virtuelle ce segment */
	Elf32_Addr	p_paddr;	/* Adresse physique / Utilisation indéfinie */
	Elf32_Word	p_filesz; 	/* Taille prise par le segment dans le fichier */
	Elf32_Word	p_memsz;	/* Taille prise par le segment dans la mémoire */
	Elf32_Word	p_flags;	/* Flags quoi... */
	Elf32_Word	p_align;	/* Si différent de 0 ou 1, on doit avoir (p_vaddr = p_offset mod p_align) */
}Elf32_Phdr;

/* Valeurs prises par p_type: */
#define PT_NULL		0		/* Unused element */
#define	PT_LOAD		1		/* Loadable segment */
#define PT_DYNAMIC	2		/* Dynamic linking information */
#define	PT_INTERP	3		/* Pathname to an interpreter */
#define	PT_NOTE		4		/* Auxiliary informations */
#define PT_SHLIB	5		/* No specified semantics */
#define PT_PHDR		6		/* Specifie location of the program header table itself */
#define PT_LOPROC	0x7000000	/* Processor specific semantics */
#define PT_HIPROC	0x7FFFFFF	/* Processor specific semantics */

/* Valeurs prises par p_flags */
#define PF_X	0x1
#define PF_W	0x2
#define PF_R	0x4

/********************************
 * 
 * SECTION HEADER
 * 
 ********************************/
 
typedef struct {
	Elf32_Word	sh_name;		/* Indice du nom de la section dans la string table */
	Elf32_Word	sh_type;		/* Type de section (voir définitions)*/
	Elf32_Word	sh_flags;		/* Attributs */
	Elf32_Addr	sh_addr;		/* Adresse en mémoire de la section si elle y a une image, 0 sinon */
	Elf32_Off	sh_offset;		/* Offset de la section dans le fichier */
	Elf32_Word	sh_size;		/* Taille de la section (sauf cas SHT_NOBITS) */
	Elf32_Word	sh_link;		/* Lien vers un autre indice de la table */
	Elf32_Word	sh_info;		/* Informations supplémentaires (Dépend du type) */
	Elf32_Word	sh_addralign;	/* Contrainte d'alignement de la section */
	Elf32_Word	sh_entsize;		/* Taille des entrées d'une table, si la section contiens une table */
}Elf32_Shdr;

/* Valeurs pour sh_type */
#define	SHT_NULL		0
#define	SHT_PROGBITS	1
#define	SHT_SYMTAB		2
#define	SHT_STRTAB		3
#define	SHT_RELA		4
#define	SHT_HASH		5
#define	SHT_DYNAMIC		6
#define	SHT_NOTE		7
#define	SHT_NOBITS		8
#define	SHT_REL			9
#define	SHT_SHLIB		10
#define	SHT_DYNSYM		11
#define	SHT_LOPROC		0x70000000
#define	SHT_HIPROC		0x7fffffff
#define	SHT_LOUSER		0x80000000
#define	SHT_HIUSER		0x8fffffff

/* Masques pour sh_flag */
#define	SHF_WRITE		1
#define	SHF_ALLOC		2
#define	SHF_EXECINSTR	4
#define	SHF_MASKPROC	0xf0000000

/***************************
 * 
 * SYMBOL TABLE
 * 
 ***************************/
 
typedef struct {
	Elf32_Word		st_name;	/* Indice du nom du symbol dans la string table des symbols */
	Elf32_Addr		st_value;	/* Valeur dépendant du contexte */
	Elf32_Word		st_size;	
	unsigned char	st_info;	/* Attributs sur le type et le binding du symbol */
	unsigned char	st_other;	/* Toujours 0, inutilisé */
	Elf32_Half		st_shndx;	/* Indice de la section contenant le symbol */
}Elf32_Sym;



/* Macros pour st_info */
#define	ELF32_ST_BIND(i)	((i)>>4)
#define	ELF32_ST_TYPE(i)	((i)&0xf)
#define	ELF32_ST_INFO(b,t)	(((b)<<4)+((t)&0xf))

/* Valeurs pour ELF32_ST_BIND */
#define	STB_LOCAL	0		/* Invisible hors du fichier objet contenant sa définition */
#define	STB_GLOBAL	1		/* Visible par tous les objet combinés à celui ci */
#define	STB_WEAK	2		/* Pareil que global, grosso modo, voir la doc pour avoir les nuances */
#define	STB_LOPROC	13
#define	STB_HIPROC	15

/* valeurs pour ELF32_ST_TYPE */
#define	STT_NOTYPE	0	/* Pas de type */
#define	STT_OBJECT	1	/* Données: variables, tableau, etc. */
#define	STT_FUNC	2	/* Fonction ou code */
#define	STT_SECTION	3	/* Le symbol est une fonction */
#define	STT_FILE	4	
#define	STT_LOPROC	13
#define	STT_HIPROC	15

/***************************
 * 
 * RELOCATIONS
 * 
 ***************************/

typedef struct {
	Elf32_Addr	r_offset;
	Elf32_Word	r_info;
} Elf32_Rel;

typedef struct {
	Elf32_Addr	r_offset;
	Elf32_Word	r_info;
	Elf32_Word	r_addend;
} Elf32_Rela;

/* Macros pour r_info */
#define ELF32_R_SYM(i)		((i)>>8)
#define ELF32_R_TYPE(i)		((unsigned char)(i))
#define ELF32_R_INFO(s,t)	(((s)<<8)+(unsigned char)(t))

/* Valeurs pour le type contenu dans r_info */
#define R_386_NONE		0
#define R_386_32		1
#define R_386_PC32		2
#define R_386_GOT32		3
#define R_386_PLT32		4
#define R_386_COPY		5
#define R_386_GLOB_DAT	6
#define R_386_JMP_SLOT	7
#define R_386_RELATIVE	8
#define R_386_GOTOFF	9
#define R_386_GOTPC		10

/***************************
 * 
 * ELF FILE
 * 
 ***************************/
 
typedef struct {
	char* name;
	
	Elf32_Ehdr* elf_header;
	
	Elf32_Phdr* pheaders;
	Elf32_Shdr* sheaders;
	
	int nb_symbols;
	Elf32_Sym* sym_table;
	
	char* string_table;
	char* symbol_string_table;
	
}Elf32_File;
/**
 * @brief charge le header principal d'un fichier elf
 * 
 * @param elf_header adresse où l'on veut charger le header
 * @param fd descripteur du fichier à charger
 * 
 * @return 1 si le fichier est bien un fichier elf valide, 0 sinon
 */
int load_efl_header(Elf32_Ehdr* elf_header, int fd);

/**
 * @brief charge un program header depuis un fichier elf
 * Charge le program header n°index depuis le fichier
 * 
 * @param program_header adresse où l'on veut charger le header
 * @param elf_header header principal du fichier
 * @param index numero du program header à charger
 * @param fd descripteur du fichier à charger
 * 
 * @return 0 si succès, -1 sinon
 */
int load_program_header(Elf32_Phdr* program_header, Elf32_Ehdr* elf_header, int index, int fd);

/**
 * @brief charge un section header depuis un fichier elf
 * Charge le section header n°index depuis le fichier
 * 
 * @param section_header adresse où l'on veut charger le header
 * @param elf_header header principal du fichier
 * @param index numero du section header à charger
 * @param fd descripteur du fichier à charger
 * 
 * @return 0 si succès, -1 sinon
 */
int load_section_header(Elf32_Shdr* section_header, Elf32_Ehdr* elf_header, int index __attribute__ ((unused)), int fd);

/**
 * @brief Calcul la taille que prendra l'exécutable dans la mémoire
 * La fonction évalue l'adresse de départ en mémoire, et l'adresse de fin, et retourne la différence
 * (il n'est pas suffisant d'additionner les tailles des différentes sections)
 * 
 * @param fd descripteur du fichier elf
 * 
 * @return taille prise initialement par l'exécutable en mémoire
 */
unsigned long int elf_size(int fd);

/**
 * @brief charge un fichier elf
 * Charge les données utiles de l'exécutable en mémoire, de façon à ce que ce soit pret à exécuter
 * 
 * @param fd descripteur du fichier à charger
 * @param dest adresse à laquelle il faut charger l'exécutable
 * 
 * @return adresse du point d'entrée de l'exécutable
 */
int load_elf(int fd, void* dest);

/**
 * @brief charge un fichier elf dans la structure Elf32_File
 * 
 * @param fd File descriptor du fichier à charger.
 * 
 * @return pointeur vers une structure Elf32_file chargée, NULL en cas d'échec
 */
Elf32_File* load_elf_file(int fd);


/**
 * @brief Trouve un symbole dans un fichier
 * 
 * @param file Fichier dans lequel on recherche le symbole
 * @param symbol Symbole cherché
 * 
 * @return symbole recherché
 */
Elf32_Sym* find_symbol(Elf32_File* file, const char* symbol);

/**
 * @brief Affiche des information sur le fichier elf
 */
void elf_info(char* name);


// XXX: pas sa place...
int exec_elf(char* name, int orphan);

#endif /* _ELF_H_ */
