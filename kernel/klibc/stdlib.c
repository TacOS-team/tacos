#include <klibc/stdlib.h>

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
	
	return ret*sig;
}

int itoa (char *buf, int base, int d) {
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
	} else if (base == 'o') {
		divisor = 8;
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

	return p - buf;
}

void itox(char *buf, int d)
{
	int i;
	int tmp = d;
	int digit;
	for(i=0; i<8; i++)
	{
		digit = (char)(tmp&0x0000000f);
		buf[7-i] = digit>9?'a'+(digit-10):'0'+(digit);
		tmp >>= 4;
	}
	buf[8] = '\0';
}


