/**
 * @file clock_graph.c
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
 * Displays an analog clock.
 */

#include <string.h>
#include <sys/syscall.h>
#include <time.h>
#include <unistd.h>
#include <vga_types.h>

#define LARGEUR 320
#define HAUTEUR 200
#define PI 3.14159f

static char buffer[LARGEUR*HAUTEUR];
static float centre[2] = {160, 100}, zero_p[2] = {0, -90};

static float aiguille_h[][2] = { {0, 8}, {-8, -0}, {8, 0}, {0, -60} };
static float aiguille_m[][2] = { {0, 10}, {-5, 0}, {5, 0}, {0, -80} };
static float aiguille_s[][2] = { {0, 0}, {0, -80} };

float cos(float angle) {
	float angle_radians = angle*(2*PI)/360;
	float result;
	asm(	"fld %1;"
		"fcos;"
		"fstp %0;" : "=g" (result) : "g" (angle_radians)
	);
	return result;
}

float sin(float angle) {
	float angle_radians = angle*(2*PI)/360;
	float result;
	asm(	"fld %1;"
		"fsin;"
		"fstp %0;" : "=g" (result) : "g" (angle_radians)
	);
	return result;
}

void clear_buffer(char* buffer) {
	memset(buffer, 0, LARGEUR*HAUTEUR); 
}
				   
void init() {
	clear_buffer(buffer);
}

void put_pixel(char c, int x, int y, char* buffer) {
	if (x >= 0 && x < LARGEUR && y >= 0 && y < HAUTEUR) {
		buffer[x+y*LARGEUR] = c;
	}
}

void put_translated_pixel(char c, int x, int y, char* buffer) {
	put_pixel(c, x + centre[0], y + centre[1], buffer);
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

void draw_translated_line(int x1, int y1, int x2, int y2, char color, char* buffer) {
	draw_line(x1 + centre[0], y1 + centre[1], x2 + centre[0], y2 + centre[1], color, buffer);
}

void rotate_point(float point[2], float theta) {
	float new_point[2];

	new_point[0] = cos(theta)*point[0] - sin(theta)*point[1];
	new_point[1] = sin(theta)*point[0] + cos(theta)*point[1];

	point[0] = new_point[0];
	point[1] = new_point[1];
}

void draw_aiguille_h(float angle, char* buffer) {
	float aiguille[4][2];
	memcpy(aiguille, aiguille_h, 4*sizeof(aiguille[0]));
	int point;
	for (point = 0; point < 4; point++) {
		rotate_point(aiguille[point], angle);
	}
	draw_translated_line(aiguille[0][0], aiguille[0][1], aiguille[1][0], aiguille[1][1], 1, buffer);
	draw_translated_line(aiguille[0][0], aiguille[0][1], aiguille[2][0], aiguille[2][1], 1, buffer);
	draw_translated_line(aiguille[3][0], aiguille[3][1], aiguille[1][0], aiguille[1][1], 1, buffer);
	draw_translated_line(aiguille[3][0], aiguille[3][1], aiguille[2][0], aiguille[2][1], 1, buffer);
}

void draw_aiguille_m(float angle, char* buffer) {
	float aiguille[4][2];
	memcpy(aiguille, aiguille_m, 4*sizeof(aiguille[0]));
	int point;
	for (point = 0; point < 4; point++) {
		rotate_point(aiguille[point], angle);
	}
	draw_translated_line(aiguille[0][0], aiguille[0][1], aiguille[1][0], aiguille[1][1], 1, buffer);
	draw_translated_line(aiguille[0][0], aiguille[0][1], aiguille[2][0], aiguille[2][1], 1, buffer);
	draw_translated_line(aiguille[3][0], aiguille[3][1], aiguille[1][0], aiguille[1][1], 1, buffer);
	draw_translated_line(aiguille[3][0], aiguille[3][1], aiguille[2][0], aiguille[2][1], 1, buffer);
}

void draw_aiguille_s(float angle, char* buffer) {
	float aiguille[2][2];
	memcpy(aiguille, aiguille_s, 2*sizeof(aiguille[0]));
	int point;
	for (point = 0; point < 2; point++) {
		rotate_point(aiguille[point], angle);
	}
	draw_translated_line(aiguille[0][0], aiguille[0][1], aiguille[1][0], aiguille[1][1], 1, buffer);
}

void draw_clock(struct tm *date, char *buffer) {
	// Points
	float point[2] = {zero_p[0], zero_p[1]};
	int minute, heure;
	for (minute = 0; minute < 60; minute++) {
		rotate_point(point, (float)360/60);
		put_translated_pixel(1, point[0], point[1], buffer);
	}
	for (heure = 0; heure < 12; heure++) {
		rotate_point(point, (float)360/12);
		int dx, dy;
		for (dy = -1; dy <= 1; dy++) {
			for (dx = -1; dx <= 1; dx++) {
				put_translated_pixel(1, (int)point[0] + dx, (int)point[1] + dy, buffer);
			}
		}
	}

	// Extraction heures, minutes, secondes
	int heures = date->tm_hour;
	int minutes = date->tm_min;
	int secondes = date->tm_sec;

	// Heures
	float angle_heures = ((heures % 12) + ((float)minutes / 60)) * ((float)360 / 12);
	draw_aiguille_h(angle_heures, buffer);
	// Minutes
	float angle_minutes = minutes * ((float)360 / 60);
	draw_aiguille_m(angle_minutes, buffer);
	// Secondes
	float angle_secondes = secondes * ((float)360 / 60);
	draw_aiguille_s(angle_secondes, buffer);
}

int main() {
	struct tm *date;
	time_t timer;

	syscall(SYS_VGASETMODE, vga_mode_320x200x256, 0, 0);
	init();
	while (1) {	
		clear_buffer(buffer);
		timer = time(NULL);
		date = gmtime(&timer);
		draw_clock(date, buffer);
		syscall(SYS_VGAWRITEBUF, (uint32_t)buffer, 0, 0);
		usleep(10000);
	}
	return 0;
}
