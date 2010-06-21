#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <process.h>
#include <string.h>
#include <video.h>

#define LARGEUR 80
#define HAUTEUR 24

static char buffer[LARGEUR*HAUTEUR + 1];



float cube[][3]= {{ 1.0f, 1.0f, -1.0f },
				 { 1.0f, -1.0f, -1.0f },
				 { -1.0f, -1.0f, -1.0f },
				 { -1.0f, 1.0f, -1.0f },
				 { 1.0f, 1.0f, 1.0f },
				 { 1.0f, -1.0f, 1.0f },
				 { -1.0f, -1.0f, 1.0f },
				 { -1.0f, 1.0f, 1.0f }};

int lignes[][2] = {{0,1}, // face du haut
				   {1,2},
				   {2,3},
				   {3,0},
				   {4,5}, // face du bas
				   {5,6},
				   {6,7},
				   {7,4},
				   {0,4}, // Arrêtes des cotés
				   {1,5},
				   {2,6},
				   {3,7}};

void clear_buffer(char* buffer)
{
	memset(buffer, ' ', LARGEUR*HAUTEUR); 
	buffer[LARGEUR*HAUTEUR] = '\0';
}
				   

void init()
{
		clear_buffer(buffer);
		disable_cursor(1);
}


void put_char(char c, int x, int y, char* buffer)
{
	if(x>=0 && x<LARGEUR && y >= 0 && y < HAUTEUR)
		buffer[x+y*LARGEUR] = c;
}

void draw_line(int x1, int y1, int x2, int y2, char* buffer)
{
	int yLonger = 0;
	int incrementVal;
	int i;
	
	int shortLen = y2-y1;
	int a_shortLen = (shortLen>=0?shortLen:shortLen*-1);
	int longLen= x2-x1;
	int a_longLen = (longLen>=0?longLen:longLen*-1);
	
	char c;
	
	float divDiff;
	
	if(a_shortLen > a_longLen)
	{
		int swap = shortLen;
		shortLen = longLen;
		longLen = swap;
		yLonger = 1;
	}
	
	incrementVal = (longLen<0?-1:1);
	divDiff = (shortLen==0?longLen:(float)longLen/(float)shortLen);
	
	if(yLonger)
	{
		if( a_longLen <= 2)
			c = '|';
		else if( a_shortLen <= 2)
			c = '-';
		else
			c = '/';
			
		for(i=0; i!=longLen; i+=incrementVal)
			put_char(c, x1+(int)((float)i/divDiff), y1+i, buffer);
	}
	else
	{
		if( a_longLen <= 2)
			c = '|';
		else if( a_shortLen <= 2)
			c = '-';
		else
			c = '\\';
			
		for(i=0; i!=longLen; i+=incrementVal)
			put_char(c, x1+i,y1+(int)((float)i/divDiff), buffer);
	}
	
}

void rotate_point(float point[3])
{
	float temp[3];
	// theta = 4°
	float cos_theta = 	0.99984769f;
	float sin_theta = 	0.01745240f;
	
	// phi = 1°
	float cos_phi = 	0.99756405f;
	float sin_phi = 	0.06975647f;
	
	temp[0] = point[0];
	temp[1] = point[1]*cos_theta - point[2]*sin_theta;
	temp[2] = point[1]*sin_theta + point[2]*cos_theta;
	
	point[0] = temp[0]*cos_phi - temp[2]*sin_phi;
	point[1] = temp[1];
	point[2] = temp[0]*sin_phi + temp[2]*cos_phi;
	/*point[0] = temp[0];
	point[1] = temp[1];
	point[2] = temp[2];*/
}

void draw_cube(char *buffer)
{
	int ligne;
	
	for(ligne = 0; ligne < 12; ligne++)
	{
		draw_line((int)(cube[lignes[ligne][0]][0]*8+40),
		          (int)(cube[lignes[ligne][0]][1]*8+12),
		          (int)(cube[lignes[ligne][1]][0]*8+40),
		          (int)(cube[lignes[ligne][1]][1]*8+12),
		          buffer);
	}
}

void refresh()
{
	printf("\033[1;1H");
	printf("%s", buffer);
}


int main_cube()
{
	int i = 0;
	init();
	rotate_point(cube[i]);
	while(1)	
	{
		refresh();
		clear_buffer(buffer);
		for(i=0; i<8; i++)
				rotate_point(cube[i]);
		draw_cube(buffer);
		
	}
	return 0;
}

int cube_launcher()
{
	exec(main_cube, "CUBE");
	return 0;
}
