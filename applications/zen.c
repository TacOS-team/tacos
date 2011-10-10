#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <unistd.h>


#define LARGEUR 80
#define HAUTEUR 24

static char buffer[LARGEUR*HAUTEUR + 1];

void clear_buffer(char* buffer)
{
	memset(buffer, ' ', LARGEUR*HAUTEUR); 
	buffer[LARGEUR*HAUTEUR] = '\0';
}

void init()
{
		clear_buffer(buffer);
		//XXX: disable_cursor(1);
}

void put_char(char c, int x, int y, char* buffer)
{
	if(x>=0 && x<LARGEUR && y >= 0 && y < HAUTEUR)
		buffer[x+y*LARGEUR] = c;
}

char get_char(int x, int y, char* buffer) {
	if(x>=0 && x<LARGEUR && y >= 0 && y < HAUTEUR)
		return buffer[x+y*LARGEUR];
	else
		return ' ';
}

void refresh()
{
	printf("\033[1;1H");
	printf("%s", buffer);
}

void update() {
	int x,y;
	for(x=0; x<LARGEUR; x++) {
		for(y=HAUTEUR-1; y>=0; y--) {
			switch(get_char(x,y,buffer)) {
				case '*':
					if(get_char(x,y+1, buffer) == ' ') {
						put_char(' ',x,y,buffer);
						put_char('*',x,y+1,buffer);
					}
					else if(get_char(x,y+1, buffer) == '*') {
						int r = rand()%100;
						if(r<37) { 
							if(get_char(x-1,y+1, buffer) == ' ') {
								put_char(' ',x,y,buffer);
								put_char('*',x-1,y+1,buffer);
							}
							else if(get_char(x-1,y, buffer) == ' ') {
								put_char(' ',x,y,buffer);
								put_char('*',x-1,y,buffer);
							}
						}
						else if(r>=37 && r<75) {
							if(get_char(x+1,y+1, buffer) == ' ') {
								put_char(' ',x,y,buffer);
								put_char('*',x+1,y+1,buffer);
							}
							else if(get_char(x+1,y, buffer) == ' ') {
								put_char(' ',x,y,buffer);
								put_char('*',x+1,y,buffer);
							}
						}
					}
					break;
				case '-':
					break;
				default:
					break;
			}
		}
	}
	
}

int main()
{
	init();
	put_char('Z',0,0,buffer);
	put_char('E',1,0,buffer);
	put_char('N',2,0,buffer);
	put_char('\\',LARGEUR/2 - 7,18,buffer);
	put_char('\\',LARGEUR/2 - 6,19,buffer);
	put_char('-', LARGEUR/2 - 5,19,buffer);
	put_char('-', LARGEUR/2 - 4,19,buffer);
	put_char('-', LARGEUR/2 - 3,19,buffer);
	put_char('-', LARGEUR/2 - 2,19,buffer);
	put_char('-', LARGEUR/2 - 1,19,buffer);
	put_char('-', LARGEUR/2,19,buffer);
	put_char('-', LARGEUR/2 + 1,19,buffer);
	put_char('-', LARGEUR/2 + 2,19,buffer);
	put_char('-', LARGEUR/2 + 3,19,buffer);
	put_char('-', LARGEUR/2 + 4,19,buffer);
	put_char('-', LARGEUR/2 + 5,19,buffer);
	put_char('/',LARGEUR/2 + 6,19,buffer);
	put_char('/',LARGEUR/2 + 7,18,buffer);
	while(1)	
	{
		//clear_buffer(buffer);
		refresh();
		put_char('*', LARGEUR/2, 1, buffer);
		usleep(50000);
		update();
		
	}
	return 0;
}


