/**
 * @file module.c
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
 * Loads & Unloads kernel modules
 */


/* Kernel */
#include <klog.h>
#include <module.h>
#include <kmalloc.h>

/* LibC */
#include <elf.h>
#include <stdio.h>

module_info_t* load_module(char* filename)
{
	module_info_t* result = NULL;
	FILE* fd = NULL;
	Elf32_File* elf_file = NULL;
	Elf32_Rel* rel = NULL;
	int nb_progbit = 0;
	int i = 0, j = 0;
	uint32_t offset;
	size_t total_size = 0;
	paddr_t ptr = (paddr_t) NULL;
	
	
	klog("Loading module %s", filename);
	fd = fopen(filename, "r");
	
	if(fd != NULL)
	{
		/* Chargement des headers du fichier elf */
		elf_file = load_elf_file(fd);
		
		if(elf_file == NULL)
			return NULL;
		
		
		result = kmalloc(sizeof(module_info_t));
		
		/* On compte le nombre de section à charger en mémoire, ainsi que la taille totale du module en mémoire */
		for(i=0; i<elf_file->elf_header->e_shnum; i++)
		{
			if(elf_file->sheaders[i].sh_type == SHT_PROGBITS)
			{
				nb_progbit++;
				total_size += elf_file->sheaders[i].sh_size;
			}
		}
		
		/* Maintenant que l'on connait la taille du module, on peut lui allouer de la place en mémoire kernel */
		result->load_addr = (paddr_t) kmalloc(total_size);
		
		/* On charge chaque sections la ou il faut */
		ptr = result->load_addr;
		for(i=0; i<elf_file->elf_header->e_shnum; i++)
		{
			/* On ne charche que les sections PROGBITS, de plus parfois certaines ont une taille nulle (.note.GNU-stack, wtf au passage), on les évite) */
			if(elf_file->sheaders[i].sh_type == SHT_PROGBITS && elf_file->sheaders[i].sh_size > 0)
			{
				fseek(fd, elf_file->sheaders[i].sh_offset, SEEK_SET);
				fread((void*)ptr, elf_file->sheaders[i].sh_size, 1 , fd);
				
				/* On met l'adresse à laquelle la section a été chargée dans le header de la section, on va s'en resservir plus tard */
				elf_file->sheaders[i].sh_addr = ptr;
				
				/* On en profite pour récupérer les info du module */
				if(strcmp(elf_file->string_table + elf_file->sheaders[i].sh_name, ".modname") == 0)
					result->name = ptr;
				if(strcmp(elf_file->string_table + elf_file->sheaders[i].sh_name, ".modversion") == 0)
					result->version = ptr;
				if(strcmp(elf_file->string_table + elf_file->sheaders[i].sh_name, ".modload") == 0)
					result->load_handler = ptr;
				if(strcmp(elf_file->string_table + elf_file->sheaders[i].sh_name, ".modunload") == 0)
					result->unload_handler = ptr;
				if(strcmp(elf_file->string_table + elf_file->sheaders[i].sh_name, ".text") == 0)
					klog("TEXT: %x:0x%x", i,ptr);

				ptr += elf_file->sheaders[i].sh_size;
			}
		}
		
		/* Maintenant, on recherche les relocations */
		for(i=0; i<elf_file->elf_header->e_shnum; i++)
		{
			if(elf_file->sheaders[i].sh_type == SHT_REL)
			{
				/* 1 section REL = 1 section à relocaliser, généralement
				 * la section précédente (j'ai jamais pu observer 
				 * le contraire, cependant si un jour ça crash sans 
				 * savoir pourquoi, ça pourrait éventuellement venir 
				 * de là 
				 */
				rel = malloc( elf_file->sheaders[i].sh_size );
				
				fseek(fd, elf_file->sheaders[i].sh_offset, SEEK_SET);
				fread((void*)rel, elf_file->sheaders[i].sh_size, 1 , fd);
				
				klog("->%d",i);
				for(j=0; j< elf_file->sheaders[i].sh_size/elf_file->sheaders[i].sh_entsize; j++)
				{
					ptr = elf_file->sheaders[i-1].sh_addr;		/* XXX NOT SAFE AT ALL */
					if(elf_file->sym_table[ELF32_R_SYM(rel[j].r_info)].st_shndx == 0) /* OK */
					{
						/* Si on est ici, c'est que l'entrée de relocation désigne un symbole du kernel et non du module, on va le chercher la où il est */
						offset = sym_to_addr(elf_file->symbol_string_table + elf_file->sym_table[ELF32_R_SYM(rel[j].r_info)].st_name);
					}
					else if(ELF32_ST_TYPE(elf_file->sym_table[ELF32_R_SYM(rel[j].r_info)].st_info)  == STT_SECTION)
					{
						/* Si on est ici, l'entrée de relocation désigne une section, ce qui veut dire que le symbole est dans une section différente de là où se trouve la relocation */
						/* Offset = offset section + offset symbole */
						offset = elf_file->sheaders[elf_file->sym_table[ELF32_R_SYM(rel[j].r_info)].st_shndx].sh_addr + *((char*)ptr+rel[j].r_offset);
						offset -= 4;
					}
					else
					{
						/* Cas le plus simple, le symbole est dans la meme section que la relocation  => faux*/
						offset = elf_file->sheaders[elf_file->sym_table[ELF32_R_SYM(rel[j].r_info)].st_shndx].sh_addr + elf_file->sym_table[ELF32_R_SYM(rel[j].r_info)].st_value;
						klog("0x%x 0x%x",elf_file->sheaders[elf_file->sym_table[ELF32_R_SYM(rel[j].r_info)].st_shndx].sh_addr, rel[j].r_offset);
						klog("rel sym 0x%x => %s (0x%x, 0x%x)", rel[j].r_info, 
														elf_file->symbol_string_table + elf_file->sym_table[ELF32_R_SYM(rel[j].r_info)].st_name,
														elf_file->sym_table[ELF32_R_SYM(rel[j].r_info)].st_value, offset);
					}
					
					ptr = rel[j].r_offset + elf_file->sheaders[i-1].sh_addr;		/* XXX NOT SAFE AT ALL */
					
					/* Reloc */
					switch(ELF32_R_TYPE(rel[j].r_info))
					{
						case R_386_32:
							*((uint32_t*)ptr) = offset;
							break;
						case R_386_PC32:
							*((uint32_t*)ptr) = offset-ptr;
							break;
						default:
							kerr("ELF32_R_TYPE=%d: Not implemented yet.",ELF32_R_TYPE(rel[i].r_info));
					}
				}
				 
				 free(rel);
			}
			
		}
		klog("modload addr = 0x%x", result->load_handler);
		
		/* Execute modload, surement pas le bon endroit */
		((void(*)())result->load_handler)();
		
	}
	else
		kerr("cannot open module %s.", filename);
		
	return result;
}
