#include <stdio.h>

int main(int argc, char** argv)
{
	int i;
	FILE* fd;
	char c;
	
	for(i=1; i<argc; i++)
	{
			fd = NULL;
			fd = fopen(argv[i], "r");
			if(fd != NULL)
			{
				c = fgetc(fd);
				while( c != EOF )
				{
					printf("%c",c);
					c = fgetc(fd);
				}
				
				//fclose(fd);
			}
			else
				printf("%s: file not found.\n", argv[i]);
	}
	return 0;
}

