/**
 * @file clock_graph.c
 *
 * @author TacOS developers 
 *
 * @section LICENSE
 *
 * Copyright (C) 2010-2014 TacOS developers.
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

#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <sys/ioctl.h>
#include <vesa_types.h>

#define WIDTH 1280
#define HEIGHT 1024
#define BYTES_PER_PIXEL 3
#define BITS_PER_PIXEL 24

#define PI 3.14159f

#define _COLOR(_p, _bpp) _p.bpp ## _bpp
#define COLOR(_p, _bpp) _COLOR(_p, _bpp)

#define VIDEO_BUFFER ((void*)0x10000000)

static int vesa_fd;

static float centre[2] = {WIDTH/2, HEIGHT/2}, zero_p[2] = {0, -(HEIGHT/2.2)};

static float aiguille_h[][2] = { {0, (HEIGHT/25.)}, {-(HEIGHT/25.), -0}, {(HEIGHT/25), 0}, {0, -(HEIGHT/3.3)} };
static float aiguille_m[][2] = { {0, (HEIGHT/20.)}, {-(HEIGHT/40.), 0}, {(HEIGHT/40.), 0}, {0, -(HEIGHT/2.5)} };
static float aiguille_s[][2] = { {0, 0}, {0, -(HEIGHT/2.5)} };

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

void put_pixel(color_t c, int x, int y) {
	if (x >= 0 && x < WIDTH && y >= 0 && y < HEIGHT) {
		memcpy(VIDEO_BUFFER + (y * WIDTH + x) * BYTES_PER_PIXEL, &COLOR(c, BITS_PER_PIXEL), sizeof(COLOR(c, BITS_PER_PIXEL)));
	}
}

void put_translated_pixel(color_t c, int x, int y) {
	put_pixel(c, x + centre[0], y + centre[1]);
}

/*
 * @see http://www.brackeen.com/vga/shapes.html
 */
void draw_line(int x1, int y1, int x2, int y2, color_t color) {
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
			put_pixel(color, px, py);
		}
	} else { /* the line is more vertical than horizontal */
		for (i = 0; i < dyabs; i++) {
			x += dxabs;
			if (x >= dyabs) {
				x -= dyabs;
				px += sdx;
			}
			py += sdy;
			put_pixel(color, px, py);
		}
	}
}

void draw_translated_line(int x1, int y1, int x2, int y2, color_t color) {
	draw_line(x1 + centre[0], y1 + centre[1], x2 + centre[0], y2 + centre[1], color);
}

void rotate_point(float point[2], float theta) {
	float new_point[2];

	new_point[0] = cos(theta)*point[0] - sin(theta)*point[1];
	new_point[1] = sin(theta)*point[0] + cos(theta)*point[1];

	point[0] = new_point[0];
	point[1] = new_point[1];
}

void draw_aiguille_h(float angle) {
	float aiguille[4][2];
	memcpy(aiguille, aiguille_h, 4*sizeof(aiguille[0]));
	int point;
	for (point = 0; point < 4; point++) {
		rotate_point(aiguille[point], angle);
	}
	color_t color;
	COLOR(color, BITS_PER_PIXEL).r = 255;
	COLOR(color, BITS_PER_PIXEL).g = 77;
	COLOR(color, BITS_PER_PIXEL).b = 182;
	draw_translated_line(aiguille[0][0], aiguille[0][1], aiguille[1][0], aiguille[1][1], color);
	draw_translated_line(aiguille[0][0], aiguille[0][1], aiguille[2][0], aiguille[2][1], color);
	draw_translated_line(aiguille[3][0], aiguille[3][1], aiguille[1][0], aiguille[1][1], color);
	draw_translated_line(aiguille[3][0], aiguille[3][1], aiguille[2][0], aiguille[2][1], color);
}

void draw_aiguille_m(float angle) {
	float aiguille[4][2];
	memcpy(aiguille, aiguille_m, 4*sizeof(aiguille[0]));
	int point;
	for (point = 0; point < 4; point++) {
		rotate_point(aiguille[point], angle);
	}
	color_t color;
	COLOR(color, BITS_PER_PIXEL).r = 255;
	COLOR(color, BITS_PER_PIXEL).g = 77;
	COLOR(color, BITS_PER_PIXEL).b = 182;
	draw_translated_line(aiguille[0][0], aiguille[0][1], aiguille[1][0], aiguille[1][1], color);
	draw_translated_line(aiguille[0][0], aiguille[0][1], aiguille[2][0], aiguille[2][1], color);
	draw_translated_line(aiguille[3][0], aiguille[3][1], aiguille[1][0], aiguille[1][1], color);
	draw_translated_line(aiguille[3][0], aiguille[3][1], aiguille[2][0], aiguille[2][1], color);
}

void draw_aiguille_s(float angle) {
	float aiguille[2][2];
	memcpy(aiguille, aiguille_s, 2*sizeof(aiguille[0]));
	int point;
	for (point = 0; point < 2; point++) {
		rotate_point(aiguille[point], angle);
	}
	color_t color;
	COLOR(color, BITS_PER_PIXEL).r = 255;
	COLOR(color, BITS_PER_PIXEL).g = 77;
	COLOR(color, BITS_PER_PIXEL).b = 182;
	draw_translated_line(aiguille[0][0], aiguille[0][1], aiguille[1][0], aiguille[1][1], color);
}

void draw_clock(struct tm *date) {
	// Points
	float point[2] = {zero_p[0], zero_p[1]};
	int minute, heure;
	color_t color;
	color.bpp24.r = 255;
	color.bpp24.g = 77;
	color.bpp24.b = 182;
	for (minute = 0; minute < 60; minute++) {
		rotate_point(point, (float)360/60);
		put_translated_pixel(color, point[0], point[1]);
	}
	for (heure = 0; heure < 12; heure++) {
		rotate_point(point, (float)360/12);
		int dx, dy;
		for (dy = -1; dy <= 1; dy++) {
			for (dx = -1; dx <= 1; dx++) {
				put_translated_pixel(color, (int)point[0] + dx, (int)point[1] + dy);
			}
		}
	}

	// Extraction heures, minutes, secondes
	int heures = date->tm_hour;
	int minutes = date->tm_min;
	int secondes = date->tm_sec;

	// Heures
	float angle_heures = ((heures % 12) + ((float)minutes / 60)) * ((float)360 / 12);
	draw_aiguille_h(angle_heures);
	// Minutes
	float angle_minutes = minutes * ((float)360 / 60);
	draw_aiguille_m(angle_minutes);
	// Secondes
	float angle_secondes = secondes * ((float)360 / 60);
	draw_aiguille_s(angle_secondes);
}

int main() {
	struct tm *date;
	time_t timer;

	vesa_fd = open("/dev/vesa", O_RDONLY);
	struct vesa_setmode_req req = { WIDTH, HEIGHT, BITS_PER_PIXEL };
	ioctl(vesa_fd, SETMODE, &req);
	while (1) {	
		timer = time(NULL);
		date = gmtime(&timer);
		draw_clock(date);
		ioctl(vesa_fd, FLUSH, 0);
		usleep(100000);
	}
	return 0;
}
