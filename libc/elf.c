/**
 * @file elf.c
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
 * Bibliotèque destinée à manipuler les fichiers exécutables ELF
 */

#include <elf.h>
#include <stdio.h>
#include <string.h>

int is_elf(Elf32_Ehdr* elf_header)
{
	int ret = 0;
	/* Pour vérifier qu'on a bien affaire à un fichier elf, il suffit de
	 * vérifier qu'on a les 4 bon magic bytes */
	if(	elf_header->e_ident[EI_MAG0] == 0x7f &&
		elf_header->e_ident[EI_MAG1] == 'E' &&
		elf_header->e_ident[EI_MAG2] == 'L' &&
		elf_header->e_ident[EI_MAG3] == 'F')
		ret = 1;
	return ret;
}

int load_efl_header(Elf32_Ehdr* elf_header, FILE* fd)
{
	fseek(fd, 0, SEEK_SET);	

	/* Remplace temporairement un fread boiteux */
	char* pointeur = (char*) elf_header;
	unsigned int i;
	for(i=0; i<sizeof(Elf32_Ehdr); i++)
	{
		*pointeur = fgetc(fd);
		pointeur++;
	}
	return is_elf(elf_header);
}

int load_program_header(Elf32_Phdr* program_header, Elf32_Ehdr* elf_header, int index __attribute__ ((unused)), FILE* fd)
{
	/* On se déplace au bon endroit dans le fichier (offset du premier header + index*taille d'un header) */
	fseek(fd, elf_header->e_phoff + index*elf_header->e_phentsize, SEEK_SET);
	
	/* Remplace temporairement un fread boiteux */
	char* pointeur = (char*)program_header;
	int i;
	for(i=0; i<elf_header->e_phentsize; i++)
	{
		*pointeur = fgetc(fd);
		pointeur++;
	}
	/* TODO: vérifier que ce header existe bien, et retourner un code d'erreur sinon... */
	return 0;
}

size_t elf_size(FILE* fd)
{
	Elf32_Ehdr elf_header;
	Elf32_Phdr p_header;
	int i;

	vaddr_t start = 0xFFFFFFFF;
	vaddr_t end = 0;
	
	if(load_efl_header(&elf_header, fd))
	{
		for(i=0; i<elf_header.e_phnum; i++)
		{
			load_program_header(&p_header, &elf_header, i, fd);
			if(p_header.p_type == PT_LOAD)
			{	
				if(p_header.p_vaddr < start)
					start = p_header.p_vaddr;
				if(p_header.p_vaddr + p_header.p_memsz > end)
					end = p_header.p_vaddr + p_header.p_memsz;
			}
		}
	}
	return (end - start);
}

int load_elf(FILE* fd, void* dest)
{
	Elf32_Ehdr elf_header;
	Elf32_Phdr p_header;

	uint32_t i;
	uint32_t j;
	
	char* pointeur = dest;

	if(load_efl_header(&elf_header, fd))
	{
		for(i=0; i< elf_header.e_phnum; i++)
		{
			load_program_header(&p_header, &elf_header, i, fd);
			
			/* Si le header correspondond à un segment à charger, on le charge! */
			if( p_header.p_type == PT_LOAD && p_header.p_vaddr >= 0x40000000)
			{
				fseek(fd, p_header.p_offset, SEEK_SET);
				j = 0;
				while(j<p_header.p_filesz)
				{
					pointeur[(p_header.p_vaddr - 0x40000000) + j] = fgetc(fd);
					/*printf("%x ",pointeur[p_header[0].p_vaddr + i]);*/
					j++;
				}
			}
		}
	}
	return elf_header.e_entry;
}


void print_elf_header_info(Elf32_Ehdr* elf_header)
{
	printf("----ELF HEADER INFO----\n");
	printf("ELF Identification: ");
	printf("%c", elf_header->e_ident[EI_MAG1]);
	printf("%c", elf_header->e_ident[EI_MAG2]);
	printf("%c ", elf_header->e_ident[EI_MAG3]);
	switch(elf_header->e_ident[EI_CLASS])
	{
		case	ELFCLASS32:
			printf("32bit ");
			break;
		case	ELFCLASS64:
			printf("64bit ");
			break;
		case	ELFCLASSNONE:
		default:
			printf("Invalid ");
	}
	printf("object (");
	switch(elf_header->e_ident[EI_DATA])
	{
		case ELFDATA2LSB:
			printf("Big-endian)\n");
			break;
		case ELFDATA2MSB:
			printf("Little-endian)\n");
			break;
		default:
			printf("Unknown data encoding)\n");
	}
	printf("filetype: ");
	switch(elf_header->e_type)
	{
		case ET_REL:
				printf("Relocatable");
				break;
		case ET_EXE:
				printf("Executable");
				break;
		case ET_DYN:
				printf("Shared object");
				break;
		case ET_CORE:
				printf("Core");
				break;
		case ET_LOPROC:
				printf("Processor-specific");
				break;
		case ET_HIPROC:
				printf("Processor-specific");
				break;
		case ET_NONE:
		default:
				printf("No type");

	}
	
	printf("\narchitecture: ");
	switch(elf_header->e_machine)
	{
		case EM_M32:
				printf("AT&T WE 32100");
				break;
		case EM_SPARC:
				printf("SPARC");
				break;
		case EM_386:
				printf("i386");
				break;
		case EM_68K:
				printf("Motorola 68000");
				break;
		case EM_88K:
				printf("Motorola 88000");
				break;
		case EM_860:
				printf("i860");
				break;
		case EM_MIPS:
				printf("MIPS RS3000");
				break;
		case EM_NONE:
		default:
				printf("None");
	}
	printf(", flags:0x%d\n", elf_header->e_flags);
	printf("ELF header size: 0x%x\n", elf_header->e_ehsize);
	printf("Entry point at 0x%x\n\n", elf_header->e_entry);
	printf("                       OFFSET\t SIZE\n");
	printf("Program header table : 0x%x\t%d*0x%x\n", elf_header->e_phoff, elf_header->e_phnum , elf_header->e_phentsize);
	printf("Section header table : 0x%x\t%d*0x%x\n", elf_header->e_shoff, elf_header->e_shnum , elf_header->e_shentsize); 

}

void print_program_header_info(Elf32_Phdr* p_header)
{
	char flags[4];
	flags[3] = '\0';

	switch(p_header->p_type)
	{
		case	PT_LOAD:
				printf("LOAD\t");
				break;
		case PT_DYNAMIC:
				printf("DYN\t");
				break;
		case	PT_INTERP:
				printf("INTERP\t");
				break;
		case	PT_NOTE:
				printf("NOTE\t");
				break;
		case PT_SHLIB:
				printf("SHLIB\t");
				break;
		case PT_PHDR:
				printf("PHDR\t");
				break;
		case PT_LOPROC:
				printf("LOPROC\t");
				break;
		case PT_HIPROC:
				printf("HIPROC\t");
				break;
		default:
				printf("UNDEF\t",p_header->p_type);
	}
	if(p_header->p_vaddr < 0x10000000)
		printf("%x\t%x\t\t%x\t%x\t",p_header->p_offset,p_header->p_vaddr,p_header->p_filesz,p_header->p_memsz);
	else
		printf("%x\t%x\t%x\t%x\t",p_header->p_offset,p_header->p_vaddr,p_header->p_filesz,p_header->p_memsz);
		
	if( p_header->p_flags & PF_X )
		flags[0] = 'X';
	else
		flags[0] = '-';
		
	if( p_header->p_flags & PF_R )
		flags[1] = 'R';
	else
		flags[1] = '-';
		
	if( p_header->p_flags & PF_W )
		flags[2] = 'W';
	else
		flags[2] = '-';
		
	printf("%s\n",flags);
	/*char* pointeur = p_header;
	int i = 0;
	int j = 0;
	for(i=0; i<4; i++)
	{
		for(j=0; j<16; j++)
		{
			printf("%.2x ",*pointeur);
			pointeur++;
		}
		printf("\n");
	}*/

}

void elf_info(FILE* fd)
{
	Elf32_Ehdr elf_header;
	Elf32_Phdr p_header;
	int i;
	
	printf("Reading ELF header...");
	load_efl_header(&elf_header, fd);
	if(is_elf(&elf_header))
	{
		printf("\033[2J");
		fflush(stdout);

		print_elf_header_info(&elf_header);
		
		fgetc(fd);
		fgetc(fd);
		
		printf("\n----PROGRAM HEADER INFO----\n");
		printf("INDEX\tTYPE\tOFF\tVADDR\t\tFSIZE\tMSIZE\tFLAGS\n");
		for(i=0; i<elf_header.e_phnum ; i++)
		{
		
			load_program_header(&p_header, &elf_header, i, fd);
			if(p_header.p_type != PT_NULL)
			{
				printf("#%d\t",i);
				print_program_header_info(&p_header);
			}
		}
	}
	else
	{
		printf("not a valid ELF file!\n");
	}
}
