/**
 * @file cube_graph.c
 *
 * @author TacOS developers 
 *
 * @section LICENSE
 *
 * Copyright (C) 2011 - TacOS developers.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 3 of
 * the License, or (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details at
 * http://www.gnu.org/copyleft/gpl.html
 *
 * You should have received a copy of the GNU General Public License 
 * along with this program; if not, see <http://www.gnu.org/licenses>.
 *
 * @section DESCRIPTION
 *
 * Displays a rotating cube.
 */

#include <fcntl.h>
#include <string.h>
#include <fcntl.h>
#include <stdio.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <vga_types.h>
#include <mouse_types.h>

#define PI 3.14159f

#define LARGEUR 320
#define HAUTEUR 200

#define FRICTION 0.9f

static char buffer[LARGEUR*HAUTEUR];

float cube[][3]= {
	{  1.0f,  1.0f, -1.0f },
	{  1.0f, -1.0f, -1.0f },
	{ -1.0f, -1.0f, -1.0f },
	{ -1.0f,  1.0f, -1.0f },
	{  1.0f,  1.0f,  1.0f },
	{  1.0f, -1.0f,  1.0f },
	{ -1.0f, -1.0f,  1.0f },
	{ -1.0f,  1.0f,  1.0f }
};

int lignes[][2] = {
	{0, 1}, // face du haut
	{1, 2},
	{2, 3},
	{3, 0},
	{4, 5}, // face du bas
	{5, 6},
	{6, 7},
	{7, 4},
	{0, 4}, // Arrêtes des cotés
	{1, 5},
	{2, 6},
	{3, 7}
};

float cos(float angle) {
	float angle_radians = angle*(2*PI)/360;
	float result;
	asm("fcos" : "=t" (result) : "0" (angle_radians));
	return result;
}

float sin(float angle) {
	float angle_radians = angle*(2*PI)/360;
	float result;
	asm("fsin" : "=t" (result) : "0" (angle_radians));
	return result;
}

void clear_buffer(char* buffer) {
	memset(buffer, 0, LARGEUR*HAUTEUR); 
}
				   
void init() {
	clear_buffer(buffer);
	//XXX: disable_cursor(1);
}

void put_pixel(char c, int x, int y, char* buffer) {
	if (x >= 0 && x < LARGEUR && y >= 0 && y < HAUTEUR) {
		buffer[x+y*LARGEUR] = c;
	}
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

void rotate_point(float point[3], float theta, float phi) {
	float temp[3];
	
	float cos_theta = 	cos(theta);
	float sin_theta = 	sin(theta);
	
	float cos_phi = 	cos(phi);
	float sin_phi = 	sin(phi);
	
	temp[0] = point[0];
	temp[1] = point[1]*cos_theta - point[2]*sin_theta;
	temp[2] = point[1]*sin_theta + point[2]*cos_theta;
	
	point[0] = temp[0]*cos_phi - temp[2]*sin_phi;
	point[1] = temp[1];
	point[2] = temp[0]*sin_phi + temp[2]*cos_phi;
}

void draw_cube(char *buffer) {
	int ligne;
	
	for (ligne = 0; ligne < 12; ligne++) {
		draw_line((int)(cube[lignes[ligne][0]][0]*50+150),
		          (int)(cube[lignes[ligne][0]][1]*50+100),
		          (int)(cube[lignes[ligne][1]][0]*50+150),
		          (int)(cube[lignes[ligne][1]][1]*50+100),
		          1, buffer);
	}
}

void draw_mouse(int x,int y) {
	draw_line(x-1, y-1, x+5, y+5, 3, buffer);
	draw_line(x, y, x+3, y, 3, buffer);
	draw_line(x, y, x, y+3, 3, buffer);
	
}

int mouse_fd = 0;

void read_mouse(mousestate_t* data) {
	if(mouse_fd == 0) {
		mouse_fd = open("/dev/mouse", O_RDONLY);
	}
	read(mouse_fd, data, sizeof(mousestate_t));
	data->y = 199 - data->y;
}



int main() {
	int i = 0;
	
	int vga_fd = open("/dev/vga", O_RDONLY);
	ioctl(vga_fd, SETMODE, (void*)vga_mode_320x200x256); 
	init();
	mousestate_t data;
	int prevx,prevy;
	
	float theta = 0.0f;
	float phi = 0.0f;
	
	while (1) {	
		clear_buffer(buffer);	
		read_mouse(&data);
		
		if(data.b1) {
			phi = (float)(data.x-prevx) * FRICTION;
			theta = (float)(data.y-prevy) * FRICTION;
		}
		else {
			phi = FRICTION * phi;
			theta = FRICTION * theta;
		}
		for (i = 0; i < 8; i++) {
			rotate_point(cube[i],theta,phi);
		}
		
		draw_cube(buffer);
		draw_mouse(data.x, data.y);
		
		ioctl(vga_fd, FLUSH, buffer);
		
		prevx = data.x;
		prevy = data.y;
		
		usleep(10000);
	}

	close(vga_fd);

	return 0;
}
