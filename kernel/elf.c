/**
 * @file elf.c
 *
 * @author TacOS developers 
 *
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
 * @brief Bibliothèque destinée à manipuler les fichiers exécutables ELF
 */

#include <elf.h>
#include <kmalloc.h>
#include <kunistd.h>
#include <fd_types.h>
#include <klibc/string.h>

static const char debug_string_sname[] = ".strtab";

static ssize_t read(int fd, void *buf, size_t count) {
	sys_read(fd, buf, &count);
	return count;
}

static int is_elf(Elf32_Ehdr* elf_header)
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

static int load_elf_header(Elf32_Ehdr* elf_header, int fd)
{
	long offset = 0;
	sys_seek(fd, &offset, SEEK_SET);	

	read(fd, elf_header, sizeof(Elf32_Ehdr));
	
	return is_elf(elf_header);
}

int load_program_header(Elf32_Phdr* program_header, Elf32_Ehdr* elf_header, int index, int fd)
{
	/* On se déplace au bon endroit dans le fichier (offset du premier header + index*taille d'un header) */
	long offset = elf_header->e_phoff + index*elf_header->e_phentsize;
	sys_seek(fd, &offset, SEEK_SET);
	
	read(fd, program_header, elf_header->e_phentsize);

	/* TODO: vérifier que ce header existe bien, et retourner un code d'erreur sinon... */
	return 0;
}

unsigned long int elf_size(int fd)
{
	Elf32_Ehdr elf_header;
	int i;

	vaddr_t start = 0xFFFFFFFF;
	vaddr_t end = 0;
	
	if(load_elf_header(&elf_header, fd))
	{
		Elf32_Phdr p_header[elf_header.e_phnum];

		long offset = elf_header.e_phoff;
		sys_seek(fd, &offset, SEEK_SET);
		read(fd, p_header, elf_header.e_phentsize * elf_header.e_phnum);

		for(i=0; i<elf_header.e_phnum; i++)
		{
			if(p_header[i].p_type == PT_LOAD)
			{	
				if(p_header[i].p_vaddr < start)
					start = p_header[i].p_vaddr;
				if(p_header[i].p_vaddr + p_header[i].p_memsz > end)
					end = p_header[i].p_vaddr + p_header[i].p_memsz;
			}
		}
	}
	return (end - start);
}

int load_elf(int fd, void* dest)
{
	Elf32_Ehdr elf_header;

	uint32_t i;
	
	char* pointeur = dest;

	if(load_elf_header(&elf_header, fd))
	{
		Elf32_Phdr p_header[elf_header.e_phnum];

		long offset = elf_header.e_phoff;
		sys_seek(fd, &offset, SEEK_SET);
		read(fd, p_header, elf_header.e_phentsize * elf_header.e_phnum);

		for(i=0; i< elf_header.e_phnum; i++)
		{
			/* Si le header correspondond à un segment à charger, on le charge! */
			if( p_header[i].p_type == PT_LOAD && p_header[i].p_vaddr >= 0x40000000)
			{
				long offset = p_header[i].p_offset;
				sys_seek(fd, &offset, SEEK_SET);
				read(fd, &(pointeur[p_header[i].p_vaddr - 0x40000000]),p_header[i].p_filesz); 
			}
		}
	}
	return elf_header.e_entry;
}

static inline char* get_string(Elf32_File* file, int n)
{
	return file->string_table + n;
}

/*
static inline char* get_debug_string(Elf32_File* file, int n)
{
	return file->symbol_string_table + n;
}
*/

Elf32_File* load_elf_file(int fd)
{
	int i;
	Elf32_File* file = NULL;
	int strtab_index = -1;
	int symtab_index = -1;
	int found = 0;
	
	if(fd != -1) 
	{
		/* Allocation de la mémoire pour la structure principale et le header du fichier */
		file = kmalloc(sizeof(Elf32_File));
		memset(file, 0, sizeof(Elf32_File));
		file->elf_header = kmalloc(sizeof(Elf32_Ehdr));
		
		if (load_elf_header(file->elf_header, fd)) 
		{
			/*file->name = strdup(filename);*/
			
			/* Allouer la mémoire pour les différents headers */
			file->pheaders = kmalloc(file->elf_header->e_phnum * sizeof(Elf32_Phdr));
			file->sheaders = kmalloc(file->elf_header->e_shnum * sizeof(Elf32_Shdr));
			
			/* Charge les program headers */
			long offset = file->elf_header->e_phoff;
			sys_seek(fd, &offset, SEEK_SET);
			read(fd, file->pheaders, file->elf_header->e_phentsize * file->elf_header->e_phnum);

			/* Charge les section headers */
			offset = file->elf_header->e_shoff;
			sys_seek(fd, &offset, SEEK_SET);
			read(fd, file->sheaders, file->elf_header->e_shentsize * file->elf_header->e_shnum);
			for (i = 0; i < file->elf_header->e_shnum; i++) {
				if(file->sheaders[i].sh_type == SHT_SYMTAB)
					symtab_index = i;
			}
			
			strtab_index = file->elf_header->e_shstrndx;
			/* Si on a trouvé une table de string, on la charge */
			if(strtab_index != SHN_UNDEF)
			{
				file->string_table = kmalloc(file->sheaders[strtab_index].sh_size);
				long offset = file->sheaders[strtab_index].sh_offset; 
				sys_seek(fd, &offset, SEEK_SET);
				read(fd, file->string_table, file->sheaders[strtab_index].sh_size);
			}
			
			/* Si on a trouvé une table des symboles, on la charge */
			
			if(symtab_index != -1)
			{
				file->sym_table = kmalloc(file->sheaders[symtab_index].sh_size);
				long offset = file->sheaders[symtab_index].sh_offset;
				sys_seek(fd, &offset, SEEK_SET);
				read(fd, file->sym_table, file->sheaders[symtab_index].sh_size);
				file->nb_symbols = file->sheaders[symtab_index].sh_size / file->sheaders[symtab_index].sh_entsize;
				
				/* Si on a une table des symboles, on devrait avoir aussi une table de string associés, on la charge ici */
				i = 0;
				while(!found && i < file->elf_header->e_shnum)
				{
					if(strcmp(debug_string_sname, get_string(file, file->sheaders[i].sh_name)) == 0)
						found = 1;
					i++;
				}
				if(found)
				{
					i--;
					file->symbol_string_table = kmalloc(file->sheaders[i].sh_size);
					long offset = file->sheaders[i].sh_offset;
					sys_seek(fd, &offset, SEEK_SET);
					read(fd, file->symbol_string_table, file->sheaders[i].sh_size);
				}
				
			} else {
				file->sym_table = NULL;
				file->symbol_string_table = NULL;
			}
			
		}
		else {
			file = NULL;
		}
	}
	return file;
}

Elf32_Sym* find_symbol(Elf32_File* file, const char* symbol)
{
	Elf32_Sym* ret = NULL;
	int i;
	
	for(i = 0; i<file->nb_symbols && ret != NULL ; i++)
	{
		if(strcmp(file->symbol_string_table + file->sym_table[i].st_name, symbol) == 0)
			ret = &(file->sym_table[i]);
	}
			
	return ret;
}

#if 0

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
	printf("String tab index: %d\n",elf_header->e_shstrndx);
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
}




void print_section_header_info(Elf32_File* e_file,int index)
{
	printf("%s\t%d\t%d\n", &(e_file->string_table[e_file->sheaders[index].sh_name]),
								e_file->sheaders[index].sh_size,
								e_file->sheaders[index].sh_entsize);
}

void print_symbols(Elf32_File* e_file)
{
	int i = 0;
	printf("---- SYMBOL TABLE ----\n");
	printf("%d symbol(s) found\n", e_file->nb_symbols);
	for(i = 0; i < e_file->nb_symbols; i++)
	{
		if(e_file->string_table[e_file->sym_table[i].st_name] != 0)
			printf("%s : 0x%x\n", &(e_file->string_table[e_file->sym_table[i].st_name]),
								e_file->sym_table[i].st_value);
	}							
}

void elf_info(char* name)
{
	
	int i;
	Elf32_File* efile;
	int fd = open(name, O_RDONLY);
	printf("Reading ELF header...\n");
	efile = load_elf_file(fd);
	
	if(efile != NULL)
	{
		//printf("\033[2J");
		fflush(stdout);

		print_elf_header_info(efile->elf_header);
		
		printf("\n----PROGRAM HEADER INFO----\n");
		printf("INDEX\tTYPE\tOFF\tVADDR\t\tFSIZE\tMSIZE\tFLAGS\n");
		for(i=0; i<efile->elf_header->e_phnum ; i++)
		{
		
			if(efile->pheaders[i].p_type != PT_NULL)
			{
				printf("#%d\t",i);
				print_program_header_info(&(efile->pheaders[i]));
			}
		}
#if 1	
		printf("\n\nN\tName\t\tType\n");
		for(i=0; i<efile->elf_header->e_shnum ; i++)
		{
		
			if(efile->sheaders[i].sh_type != SHT_NULL)
			{
				printf("#%d\t",i);
				print_section_header_info(efile, i);
			}
		}
#endif
		//print_symbols(efile);
	}
	else
	{
		printf("not a valid ELF file!\n");
	}
}

/* TODO
Utiliser la bonne string table selon l'utilisation (.debug_str pour les section, .debug_pubtypes pour les symboles visiblement)
*/

#endif
