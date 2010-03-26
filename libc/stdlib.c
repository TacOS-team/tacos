#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <types.h>
#include <pagination.h>
#include <heap.h>
#include <errno.h>

heap_t *heap = NULL;
//void initHeap(heap_t* h, cmp_func_type cmp);

int init_malloc()
{
	struct page_directory_entry * pagination_kernel;

  heap = (heap_t *) memory_reserve_page_frame(); 

  //initHeap(heap, ); 
}

void *malloc(size_t size)
{
  if(heap == NULL && init_malloc() == -1)
    return -1;
}

void free(void *addr)
{

}

static int digitvalue(char c)
{
	if(c>='0' && c<='9')
		return c-'0';
	if(c>='a' && c<='z')
		return 10+c-'a';
	if(c>='A' && c<='Z')
		return 10+c-'A';
	return -1;
}

unsigned long int strtoul(const char *nptr, char **endptr, int base)
{
	unsigned long int ret = 0;
	int signe = 1;
	
	//on ignore les espaces
	while(isspace(*(nptr))) nptr++;
	
	//on regarde le signe facultatif
	if(*nptr == '+')
		nptr++;
	else if(*nptr == '-')
		nptr++;
	
	//en magic base on regarde le debut pour determiner le type de base
	if(base == 0 || base == 16)
	{
		// on pass een base 16 si ca commence par 0x
		if(strncmp(nptr,"0x",2) == 0)
		{
			base = 16;
			nptr+=2;
		}
		else if(base == 0)
		{
			if(*nptr == '0')
				base = 8;
			else
				base = 10;
		}
	}
	
	if(base < 2 || base > 36)
	{
		errno = 1;
		return 0;
	}
	
	//on parse en base n
	int digit;
	for(digit = digitvalue(*nptr++) ; digit>=0 && digit<base ; digit = digitvalue(*nptr++))
	{
		ret = digit+base*ret;
	}
	
	*endptr = nptr-1;
	
	return ret;
}

long int strtol(const char* nptr, char** endptr, int base)
{
	long int ret = 0;
	unsigned long int uret;
	int signe = 1;
	const char* bckp = nptr;
	//on ignore les espaces
	while(isspace(*(bckp))) bckp++;
	
	//on regarde le signe facultatif
	if(*bckp == '+')
	{
		signe = 1;
		bckp++;
	}
	else if(*bckp == '-')
	{
		signe = -1;
		bckp++;
	}
	
	uret = strtoul(nptr,endptr,base);
	ret = signe*(long int)uret;
	return ret;
}
