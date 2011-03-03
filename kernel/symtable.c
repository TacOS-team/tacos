#include <elf.h>
#include <symtable.h>
#include <klog.h>

/**TODO Le tri du tableau c'est pas du tout optimal, il vaudrait mieux utiliser une liste chainée */

typedef struct
{
	char* name;
	paddr_t addr;
}symbol_t;

static symbol_t* symtable = NULL;
static int nb_symbols = 0;
const char undef[]="???";
const char userland[]= "@userland";

static void swap_elements(int i, int j)
{
	symbol_t temp;
	temp.name = symtable[i].name;
	temp.addr = symtable[i].addr;
	
	symtable[i].name = symtable[j].name;
	symtable[i].addr = symtable[j].addr;
	
	symtable[j].name = temp.name;
	symtable[j].addr = temp.addr;
}

static void sort_array()
{
	int no_swap = 1;
	int i;
	
	do
	{
		no_swap = 1;
		for(i = 0; i<nb_symbols-1; i++)
		{
			if(symtable[i].addr > symtable[i+1].addr)
			{
				swap_elements(i, i+1);
				no_swap = 0;
			}
		}
	}while(!no_swap);
}

int load_symtable()
{
	Elf32_File* file;
	int i,j, len;
	int nb_func = 0;
	char* string;
	
	/* Chargement des headers de kernel.bin */
	file = load_elf_file("fd0:/system/kernel.bin");
	
	/* Compte le nombre de symboles de fonctions, optimisé en espace précompilation*/
	for(i=0; i<file->nb_symbols; nb_func+=(ELF32_ST_TYPE(file->sym_table[i++].st_info) == STT_FUNC));
	nb_symbols = nb_func;
	klog("%d func symbols found", nb_func);
	
	/* Allocation de la table des symboles dans le kernel */
	symtable = kmalloc(nb_func * sizeof(symbol_t));
	
	/* On ne récupère que les symboles de fonction, osef du reste pour le moment
	 * Sinon, on pourrait ajouter un champ à symbol_t pour identifier le type de symbol */
	for(i=0,j=0; i<file->nb_symbols; i++)
	{
		/* Si le symbole est une fonction... */
		if(ELF32_ST_TYPE(file->sym_table[i].st_info) == STT_FUNC)
		{
			string = file->string_table + file->sym_table[i].st_name;
			len = strlen(string);
			
			/* Pas de strdup dans le kernel, on alloue et copie "à la main" */
			symtable[j].name = kmalloc(len+1);
			memcpy(symtable[j].name, string , len+1); 
			
			symtable[j].addr = file->sym_table[i].st_value;
			
			
			j++;
		}
		//file->sym_table[i].st_name;
		//file->sym_table[i].st_value;
	}
	sort_array();
}

paddr_t sym_to_addr(char* symbol)
{
	int found = 0;
	int i = 0;
	paddr_t res = 0;
	while(!found & i < nb_symbols)
	{
		if(strcmp(symbol, symtable[i]) == 0)
		{
			found = 1;
			res = symtable[i].addr;
		}
		i++;
	}
	return res;
}

char* addr_to_sym(paddr_t addr)
{
	int found = 0;
	int i = 0;
	char* res = NULL;
	while(!found & i < nb_symbols-1)
	{
		if(addr >= symtable[i].addr && addr < symtable[i+1].addr)
		{
			found = 1;
			res = symtable[i].name;
		}
		i++;
	}
	if(addr > 0x4000000)
		res = userland;
	else if(!found)
		res = undef;
	
	return res;
}
