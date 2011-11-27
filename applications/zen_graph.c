#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <time.h>
#include <unistd.h>
#include <vga_types.h>

#define LARGEUR 320
#define HAUTEUR 200

static char buffer[LARGEUR*HAUTEUR + 1];

void clear_buffer(char* buffer)
{
	memset(buffer, 0, LARGEUR*HAUTEUR); 
	buffer[LARGEUR*HAUTEUR] = '\0';
}

void init()
{
		clear_buffer(buffer);
		//XXX: disable_cursor(1);
}

void put_pixel(char c, int x, int y, char* buffer)
{
	if(x>=0 && x<LARGEUR && y >= 0 && y < HAUTEUR)
		buffer[x+y*LARGEUR] = c;
}

char get_char(int x, int y, char* buffer) {
	if(x>=0 && x<LARGEUR && y >= 0 && y < HAUTEUR)
		return buffer[x+y*LARGEUR];
	else
		return 0;
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
				case 1:
					if(get_char(x,y+1, buffer) == 0) {
						put_pixel(0,x,y,buffer);
						put_pixel(1,x,y+1,buffer);
					}
					else if(get_char(x,y+1, buffer) == 1) {
						int r = rand()%100;
						if(r<37) { 
							if(get_char(x-1,y+1, buffer) == 0) {
								put_pixel(0,x,y,buffer);
								put_pixel(1,x-1,y+1,buffer);
							}
							else if(get_char(x-1,y, buffer) == 0) {
								put_pixel(0,x,y,buffer);
								put_pixel(1,x-1,y,buffer);
							}
						}
						else if(r>=37 && r<75) {
							if(get_char(x+1,y+1, buffer) == 0) {
								put_pixel(0,x,y,buffer);
								put_pixel(1,x+1,y+1,buffer);
							}
							else if(get_char(x+1,y, buffer) == 0) {
								put_pixel(0,x,y,buffer);
								put_pixel(1,x+1,y,buffer);
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

void draw_plate(char* buffer, char color, int x, int y) {
	put_pixel(color,x - 7,y-1,buffer);
	put_pixel(color,x - 6,y,buffer);
	put_pixel(color, x - 5,y,buffer);
	put_pixel(color, x - 4,y,buffer);
	put_pixel(color, x - color,y,buffer);
	put_pixel(color, x - 2,y,buffer);
	put_pixel(color, x - 1,y,buffer);
	put_pixel(color, x,y,buffer);
	put_pixel(color, x + 1,y,buffer);
	put_pixel(color, x + 2,y,buffer);
	put_pixel(color, x + color,y,buffer);
	put_pixel(color, x + 4,y,buffer);
	put_pixel(color, x + 5,y,buffer);
	put_pixel(color,x + 6,y,buffer);
	put_pixel(color,x + 7,y-1,buffer);
}

/*
 * @see http://www.brackeen.com/vga/shapes.html
 */
void draw_line(int x1, int y1, int x2, int y2, char color, char *buffer) {
	int i, dx, dy, sdx, sdy, dxabs, dyabs, x, y, px, py;

	dx = x2 - x1; /* the horizontal distance of the line */
	dy = y2 - y1; /* the vertical distance of the line */
	dxabs = (dx >= 0 ? dx : -dx);
	dyabs = (dy >= 0 ? dy : -dy);
	sdx = (dx >= 0 ? 1 : -1);
	sdy = (dy >= 0 ? 1 : -1);
	x = dyabs >> 1;
	y = dxabs >> 1;
	px = x1;
	py = y1;

	if (dxabs >= dyabs) { /* the line is more horizontal than vertical */
		for (i = 0; i < dxabs; i++) {
			y += dyabs;
			if (y >= dxabs) {
				y -= dxabs;
				py += sdy;
			}
			px += sdx;
			put_pixel(color, px, py, buffer);
		}
	} else { /* the line is more vertical than horizontal */
		for (i = 0; i < dyabs; i++) {
			x += dxabs;
			if (x >= dyabs) {
				x -= dyabs;
				px += sdx;
			}
			py += sdy;
			put_pixel(color, px, py, buffer);
		}
	}
}

/*
 
***** ***** *   *
   *  *     **  *
  *   ***   * * *
 *    *     *  **
***** ***** *   *
 
*/
void zen(char* buffer, char color, int x,int y) {
	put_pixel(color,x+0,y+0,buffer);
	put_pixel(color,x+1,y+0,buffer);
	put_pixel(color,x+2,y+0,buffer);
	put_pixel(color,x+3,y+0,buffer);
	put_pixel(color,x+4,y+0,buffer);
	
	put_pixel(color,x+6,y+0,buffer);
	put_pixel(color,x+7,y+0,buffer);
	put_pixel(color,x+8,y+0,buffer);
	put_pixel(color,x+9,y+0,buffer);
	put_pixel(color,x+10,y+0,buffer);
	
	put_pixel(color,x+12,y+0,buffer);
	put_pixel(color,x+16,y+0,buffer);
/***/
	put_pixel(color,x+3,y+1,buffer);
	
	put_pixel(color,x+6,y+1,buffer);
	
	put_pixel(color,x+12,y+1,buffer);
	put_pixel(color,x+13,y+1,buffer);
	put_pixel(color,x+16,y+1,buffer);
/***/
	put_pixel(color,x+2,y+2,buffer);
	
	put_pixel(color,x+6,y+2,buffer);
	put_pixel(color,x+7,y+2,buffer);
	put_pixel(color,x+8,y+2,buffer);
	
	put_pixel(color,x+12,y+2,buffer);
	put_pixel(color,x+14,y+2,buffer);
	put_pixel(color,x+16,y+2,buffer);
/***/
	put_pixel(color,x+1,y+3,buffer);
	
	put_pixel(color,x+6,y+3,buffer);
	
	put_pixel(color,x+12,y+3,buffer);
	put_pixel(color,x+15,y+3,buffer);
	put_pixel(color,x+16,y+3,buffer);
/***/
	put_pixel(color,x+0,y+4,buffer);
	put_pixel(color,x+1,y+4,buffer);
	put_pixel(color,x+2,y+4,buffer);
	put_pixel(color,x+3,y+4,buffer);
	put_pixel(color,x+4,y+4,buffer);
	
	put_pixel(color,x+6,y+4,buffer);
	put_pixel(color,x+7,y+4,buffer);
	put_pixel(color,x+8,y+4,buffer);
	put_pixel(color,x+9,y+4,buffer);
	put_pixel(color,x+10,y+4,buffer);
	
	put_pixel(color,x+12,y+4,buffer);
	put_pixel(color,x+16,y+4,buffer);


}

int main()
{
	int vga_fd = open("/dev/vga", O_RDONLY);
	ioctl(vga_fd, SETMODE, (void*)vga_mode_320x200x256);
	init();

	draw_line(LARGEUR/2 - 20, 100, LARGEUR/2 + 20, 100, 15, buffer);	
	draw_line(LARGEUR/2 - 30, 120, LARGEUR/2 - 10, 120, 15, buffer);	
	draw_line(LARGEUR/2 + 10, 140, LARGEUR/2 + 30, 140, 15, buffer);

	zen(buffer, 1, LARGEUR/5,0);
	
	zen(buffer, 15, LARGEUR/5,HAUTEUR/2);

	while(1)	
	{
		
		refresh();
		put_pixel(1, LARGEUR/2+ rand()%10 - 5, 1, buffer);
		
		update();
		ioctl(vga_fd, FLUSH, buffer);
	}

	close(vga_fd);

	return 0;
}


