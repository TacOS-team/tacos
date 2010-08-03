#include <elf.h>
#include <stdio.h>
#include <string.h>

int is_elf(Elf32_Ehdr* elf_header)
{
	int ret = 0;

	if(	elf_header->e_ident[EI_MAG0] == 0x7f &&
		elf_header->e_ident[EI_MAG1] == 'E' &&
		elf_header->e_ident[EI_MAG2] == 'L' &&
		elf_header->e_ident[EI_MAG3] == 'F')
		ret = 1;
	return ret;
}

int load_efl_header(Elf32_Ehdr* elf_header, FILE* fd)
{
	/* On reset le header */
	memset(elf_header, 0, sizeof(Elf32_Ehdr));
	
	/* Puis on lis tout simplement */
	
	int i = fread(elf_header, 1, 100, fd);
	
	/* Remplace temporairement un fread boiteux */
	/*char* pointeur = elf_header;
	int i;
	for(i=0; i<sizeof(Elf32_Ehdr)-2; i++)
	{
		*pointeur = fgetc(fd);
		pointeur++;
	}
	printf("%x bytes read.\n",i);*/
	return is_elf(elf_header);
}

int load_program_header(Elf32_Phdr* program_header, Elf32_Ehdr* elf_header, int index __attribute__ ((unused)), FILE* fd)
{
	/* Normalement, on fait un fseek ici pour accéder à l'offset du program header.
	 * En attendant, on va concidérer qu'on les lis les un après les autres
	 */
		/* Remplace temporairement un fread boiteux */
	char* pointeur = (char*)program_header;
	int i;
	for(i=0; i<elf_header->e_phentsize; i++)
	{
		*pointeur = fgetc(fd);
		pointeur++;
	}
	return 0;
}

int load_elf(FILE* fd, void* dest)
{
	Elf32_Ehdr elf_header;
	Elf32_Phdr p_header[2];
	uint32_t current_offset = 0;
	char c;
	uint32_t i;
	char* pointeur = dest;
	
	/*int prog_offset[2];*/

	if(load_efl_header(&elf_header, fd))
	{
			fgetc(fd);
	fgetc(fd);
		load_program_header(&p_header[0],&elf_header, 0, fd);
		load_program_header(&p_header[1],&elf_header, 1, fd);
		
		current_offset = elf_header.e_ehsize + (2*elf_header.e_phentsize);
		printf("\n%x - %x\n",current_offset, p_header[0].p_offset);
		while(current_offset < p_header[0].p_offset)
		{
			fgetc(fd);
			current_offset++;
		}
		i = 0;
		
		while(i<p_header[0].p_filesz)
		{
			pointeur[p_header[0].p_vaddr + i] = fgetc(fd);
			printf("%x ",pointeur[p_header[0].p_vaddr + i]);
			i++;
			current_offset++;
		}
		
		printf("\n%x - %x\n",current_offset, p_header[1].p_offset);
		while(current_offset < p_header[1].p_offset)
		{
			fgetc(fd);
			current_offset++;
		}
		i = 0;
		while(i<p_header[1].p_filesz)
		{
			c = fgetc(fd);
			if(c!=0)
				printf("%c",c);
			i++;
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
		printf("Ok!\n");
	
		printf("\033[2J");
		fflush(stdout);

		print_elf_header_info(&elf_header);
		
		fgetc(fd);
		fgetc(fd);
		
		printf("\n----PROGRAM HEADER INFO----\n");
		printf("INDEX\tTYPE\tOFF\tVADDR\tFSIZE\tMSIZE\tFLAGS\n");
		for(i=0; i<elf_header.e_phnum ; i++)
		{
		
			load_program_header(&p_header, &elf_header, 0, fd);
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
