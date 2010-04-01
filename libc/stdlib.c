#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <types.h>
#include <pagination.h>
#include <heap.h>
#include <errno.h>
#include <ctype.h>

void *malloc(size_t size)
{
  return kmalloc(size);
}

void *calloc(size_t nmemb, size_t size)
{
  int i;
  uint8_t *p;
  
  if(nmemb == 0 || size == 0)
    return NULL;

  p = (uint8_t *) kmalloc(nmemb * size);
  for(i=0 ; i<nmemb*size ; i++)
    *(p+i) = 0;

  return p;
}

void free(void *addr)
{
  return kfree(addr);
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
