/**
 * @file elf.h
 *
 * @author TacOS developers 
 *
 * Maxime Cheramy <maxime81@gmail.com>
 * Nicolas Floquet <nicolasfloquet@gmail.com>
 * Benjamin Hautbois <bhautboi@gmail.com>
 * Ludovic Rigal <ludovic.rigal@gmail.com>
 * Simon Vernhes <simon@vernhes.eu>
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

#ifndef _ELF_H_
#define _ELF_H_

/**
 * @file elf.h
 * 
 */

#include <libio.h>

typedef unsigned long int 	Elf32_Addr;
typedef unsigned short int 	Elf32_Half;
typedef unsigned long int 	Elf32_Off;
typedef signed long int 	Elf32_Sword;
typedef	unsigned long int	Elf32_Word;


/*******************************
 * 
 * ELF HEADER
 * 
 *******************************/
 
#define EI_NIDENT	16

typedef struct{
	unsigned char	e_ident[EI_NIDENT];	/* ELF Identification */
	Elf32_Half		e_type;				/* Type de fichier ELF */
	Elf32_Half		e_machine;			/* Architecture de la machine */
	Elf32_Word		e_version;			/* Version du fichier objet */
	Elf32_Addr		e_entry;			/* Adresse virtuelle du point d'entrée */
	Elf32_Off		e_phoff;			/* Offset du program header dans le fichier (0 si aucun program header)*/
	Elf32_Off		e_shoff;			/* Offset du section header dans le fichier (0 si aucun section header)*/
	Elf32_Word		e_flags;			/* Flags spécifiques à la machine */
	Elf32_Half		e_ehsize;			/* Taille du header ELF */
	Elf32_Half		e_phentsize;		/* Taille d'une entrée dans la table des program header (toutes les entrées ont la même taille) */
	Elf32_Half		e_phnum;			/* Nombre d'entrées dans la table des program header */
	Elf32_Half		e_shentsize;		/* Taille d'une entrée dans la table des section header */
	Elf32_Half		e_shnum;			/* Nombre d'entrées dans la table des section header */
	Elf32_Half		e_shstrndx;			/* Indice de l'entrée de la table des section header correspondant à la table des strings, si aucune entrée de ce genre, SHN_UNDEF */
}Elf32_Ehdr;

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
	
/**
 * @brief charge le header principal d'un fichier elf
 * 
 * @param elf_header adresse où l'on veut charger le header
 * @param fd descripteur du fichier à charger
 * 
 * @return 1 si le fichier est bien un fichier elf valide, 0 sinon
 */
int load_efl_header(Elf32_Ehdr* elf_header, FILE* fd);

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
int load_program_header(Elf32_Phdr* program_header, Elf32_Ehdr* elf_header, int index, FILE* fd);

/**
 * @brief Calcul la taille que prendra l'exécutable dans la mémoire
 * La fonction évalue l'adresse de départ en mémoire, et l'adresse de fin, et retourne la différence
 * (il n'est pas suffisant d'additionner les tailles des différentes sections)
 * 
 * @param fd descripteur du fichier elf
 * 
 * @return taille prise initialement par l'exécutable en mémoire
 */
size_t elf_size(FILE* fd);

/**
 * @brief charge un fichier elf
 * Charge les données utiles de l'exécutable en mémoire, de façon à ce que ce soit pret à exécuter
 * 
 * @param fd descripteur du fichier à charger
 * @param dest adresse à laquelle il faut charger l'exécutable
 * 
 * @return adresse du point d'entrée de l'exécutable
 */
int load_elf(FILE* fd, void* dest);

/**
 * @brief Affiche des information sur le fichier elf
 */
void elf_info(FILE* fd);

#endif /* _ELF_H_ */
