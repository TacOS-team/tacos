#include <klibc/string.h>
#include <kprocess.h>
#include <kmalloc.h>
#include <symtable.h>
#include <klog.h>

#include <kfcntl.h>

#include <vfs.h>

/**TODO Le tri du tableau c'est pas du tout optimal, il vaudrait mieux utiliser une liste chainée */

const char undef[]="???";

symbol_table_t* ksymtable = NULL;

static void swap_elements(symbol_table_t* table, int i, int j)
{
	symbol_t temp;
	temp.name = table->symbols[i].name;
	temp.addr = table->symbols[i].addr;
	
	table->symbols[i].name = table->symbols[j].name;
	table->symbols[i].addr = table->symbols[j].addr;
	
	table->symbols[j].name = temp.name;
	table->symbols[j].addr = temp.addr;
}

static void sort_symbols(symbol_table_t* table) {
	int no_swap = 1;
	int i;
	
	do
	{
		no_swap = 1;
		for(i = 0; i<table->count-1; i++)
		{
			if(table->symbols[i].addr > table->symbols[i+1].addr)
			{
				swap_elements(table, i, i+1);
				no_swap = 0;
			}
		}
	}while(!no_swap);
}

symbol_table_t* load_symtable(Elf32_File* file)
{
	int i,j, len;
	int nb_func = 0;
	char* string;
	symbol_table_t* table;
	
	if(file == NULL)
		return NULL;
	
	table = kmalloc(sizeof(symbol_table_t));
	
	/* Compte le nombre de symboles de fonctions, optimisé en espace précompilation*/
	for(i=0; i<file->nb_symbols; nb_func+=(ELF32_ST_TYPE(file->sym_table[i++].st_info) == STT_FUNC));
	
	table->count = nb_func;
	
	/* Allocation de la table des symboles dans le kernel */
	table->symbols = kmalloc(nb_func * sizeof(symbol_t));
	
	/* On ne récupère que les symboles de fonction, osef du reste pour le moment
	 * Sinon, on pourrait ajouter un champ à symbol_t pour identifier le type de symbol */
	for(i=0,j=0; i<file->nb_symbols; i++)
	{
		/* Si le symbole est une fonction... */
		if(ELF32_ST_TYPE(file->sym_table[i].st_info) == STT_FUNC)
		{
			string = file->symbol_string_table + file->sym_table[i].st_name;
			len = strlen(string);
			
			/* Pas de strdup dans le kernel, on alloue et copie "à la main" */
			table->symbols[j].name = kmalloc(len+1);
			memcpy(table->symbols[j].name, string , len+1); 
			
			table->symbols[j].addr = file->sym_table[i].st_value;
			
			j++;
		}
	}
	//print_symbols(table);
	sort_symbols(table);
	return table;
}

void load_kernel_symtable() {
	int fd;
 	sys_open(&fd, "/core/system/kernel.bin", O_RDONLY);
	
	if (fd >= 0) {
		Elf32_File* file;
		file = load_elf_file(fd);
		ksymtable = load_symtable(file);
		uint32_t ret;
		sys_close(fd, &ret, 0);
	} else {
		klog("kernel.bin non trouvé !");
	}
}

paddr_t sym_to_addr(symbol_table_t* table, char* symbol)
{
	int found = 0;
	int i = 0;
	paddr_t res = 0;
	while((!found) && (i < table->count))
	{
		if(strcmp(symbol, table->symbols[i].name) == 0)
		{
			found = 1;
			res = table->symbols[i].addr;
		}
		i++;
	}
	return res;
}

char* addr_to_sym(symbol_table_t* table, paddr_t addr)
{
	int found = 0;
	int i = 0;
	char* res = NULL;
        if(table != NULL) {
		while(!found && (i < table->count-1))
		{	
			if(addr >= table->symbols[i].addr && addr < table->symbols[i+1].addr)
			{
				found = 1;
				res = table->symbols[i].name;
			}
			i++;
		}
		/*if(addr > USER_PROCESS_BASE)
			res = (char*) userland;*/
		if(!found)
			res = (char*) undef;
	} else {
		res = (char*) undef;
	}
	return res;
}
