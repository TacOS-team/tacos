/**
 * @file cube_graph.c
 *
 * @author TacOS developers 
 *
 * Maxime Cheramy <maxime81@gmail.com>
 * Nicolas Floquet <nicolasfloquet@gmail.com>
 * Benjamin Hautbois <bhautboi@gmail.com>
 * Ludovic Rigal <ludovic.rigal@gmail.com>
 * Simon Vernhes <simon@vernhes.eu>
 *
 * @section LICENSE
 *
 * Copyright (C) 2010 - TacOS developers.
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
 * Description de ce que fait le fichier
 */

#include <string.h>
#include <sys/syscall.h>
#include <unistd.h>
#include <vga_types.h>

#define LARGEUR 320
#define HAUTEUR 200

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

void rotate_point(float point[3]) {
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

int main() {
	syscall(SYS_VGASETMODE, vga_mode_320x200x256, 0, 0);
	init();
	while (1) {	
		clear_buffer(buffer);
		int i = 0;
		for (i = 0; i < 8; i++) {
			rotate_point(cube[i]);
		}
		draw_cube(buffer);
		syscall(SYS_VGAWRITEBUF, (uint32_t)buffer, 0, 0);
		usleep(10000);
	}
	return 0;
}
