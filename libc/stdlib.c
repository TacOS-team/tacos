#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <pagination.h>
#include <heap.h>
#include <errno.h>
#include <ctype.h>
#include <string.h>
#include <malloc.h>

void *calloc(size_t nmemb, size_t size)
{
  unsigned int i;
  uint8_t *p;
  
  if(nmemb == 0 || size == 0)
    return NULL;

  p = (uint8_t *) malloc(nmemb * size);
  for(i=0 ; i<nmemb*size ; i++)
    *(p+i) = 0;

  return p;
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
	
	*endptr =(char*) nptr-1;
	
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

int atoi(const char* __nptr)
{
	int ret = 0;
	int sig = 1;
	
	if(*__nptr == '-')
	{
		__nptr++;
		sig = -1;
	}
	else if(*__nptr == '+')
		__nptr++;
	
	while(*__nptr)
	{
		if(*__nptr >= '0' && *__nptr<='9')
			ret = 10*ret+(*__nptr-'0');
		else
			return -1;
		
		__nptr++;
	}
	
	return ret;
}

void itoa (char *buf, int base, int d) {
	char *p = buf;
	char *p1, *p2;
	unsigned long ud = d;
	int divisor = 10;

	/* If %d is specified and D is minus, put `-' in the head. */
	if (base == 'd' && d < 0) {
		*p++ = '-';
		buf++;
		ud = -d;
	} else if (base == 'x') {
		divisor = 16;
	}

	/* Divide UD by DIVISOR until UD == 0. */
	do {
		int remainder = ud % divisor;

		*p++ = (remainder < 10) ? remainder + '0' : remainder + 'a' - 10;
	} while (ud /= divisor);

	/* Terminate BUF. */
	*p = 0;

	/* Reverse BUF. */
	p1 = buf;
	p2 = p - 1;
	while (p1 < p2)
	{
		char tmp = *p1;
		*p1 = *p2;
		*p2 = tmp;
		p1++;
		p2--;
	}
}

// Choisi avec un dé équilibré.
// Garanti d'être aléatoire.
static unsigned long random_number = 4;

int rand(void) {
	random_number = 16807 * random_number;
	return random_number % RAND_MAX;
}

void srand(unsigned int seed) {
	random_number = seed;
}
