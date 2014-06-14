/**
 * @file video.c
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
 * Description de ce que fait le fichier
 */

#include <types.h>
#include <ioports.h>
#include <drivers/video.h>

/**
 *  The video memory address. 
 */
#define BASE_VGA_VIDEO                   0xB8000

#define CRT_REG_INDEX 0x3d4
#define CRT_REG_DATA  0x3d5
#define CURSOR_POS_MSB 0x0E
#define CURSOR_POS_LSB 0x0F

#define NB_VGA_PAGES 4 /**< Nombre de pages possibles par le driver VGA dans le mode texte. */

static struct vga_page_t pages[NB_VGA_PAGES];
static int current_page;

/**
 * Table de conversion latin1 vers cp437 (celui utilisé pour l'affichage)
 */
unsigned char convert_latin1_cp437[] = {
  0,   1,   2,   3,   4,   5,   6,   7,   8,   9,  10,  11,  12,  13,  14,  15,
 16,  17,  18,  19,  20,  21,  22,  23,  24,  25,  26,  27,  28,  29,  30,  31,
 32,  33,  34,  35,  36,  37,  38,  39,  40,  41,  42,  43,  44,  45,  46,  47,
 48,  49,  50,  51,  52,  53,  54,  55,  56,  57,  58,  59,  60,  61,  62,  63,
 64,  65,  66,  67,  68,  69,  70,  71,  72,  73,  74,  75,  76,  77,  78,  79,
 80,  81,  82,  83,  84,  85,  86,  87,  88,  89,  90,  91,  92,  93,  94,  95,
 96,  97,  98,  99, 100, 101, 102, 103, 104, 105, 106, 107, 108, 109, 110, 111,
112, 113, 114, 115, 116, 117, 118, 119, 120, 121, 122, 123, 124, 125, 126, 127,
186, 205, 201, 187, 200, 188, 204, 185, 203, 202, 206, 223, 220, 219, 254, 242,
179, 196, 218, 191, 192, 217, 195, 180, 194, 193, 197, 176, 177, 178, 213, 159,
255, 173, 155, 156, 207, 157, 221, 21, 249, 184, 166, 174, 170, 240, 169, 238,
248, 241, 253, 252, 239, 230, 244, 250, 247, 251, 167, 175, 172, 171, 243, 168,
183, 181, 182, 199, 142, 143, 146, 128, 212, 144, 210, 211, 222, 214, 215, 216,
209, 165, 227, 224, 226, 229, 153, 158, 190, 235, 233, 234, 154, 237, 232, 225,
133, 160, 131, 198, 132, 134, 145, 135, 138, 130, 136, 137, 141, 161, 140, 139,
208, 164, 149, 162, 147, 228, 148, 246, 189, 151, 163, 150, 129, 236, 231, 152
};


void init_video() {
	// Initialisation du Attribute Mode Control Register.
	// 0x08: Blink, 0x04: LGE, 0x02: Mono, 0x01: ATGE
	(void) inb(0x3DA);
	outb(0x10, 0x3C0);
	outb(0x04, 0x3C0);
	outb(0x20, 0x3C0);
	(void) inb(0x3DA);

	/* Initialisation des pages vidéos. */
	int i;
	for (i = 0; i < NB_VGA_PAGES; i++) {
		pages[i].front_buffer = (volatile x86_video_mem*) (BASE_VGA_VIDEO + 2
				* i * 0x1000);
		pages[i].back_buffer = (volatile x86_video_mem*) (BASE_VGA_VIDEO + ((2
				* i) + 1) * 0x1000);
	}
	current_page = 0;
}

static void offset_video(int offset) {
	/* CRT index port => demande l'accès au registre 0xa ("addr start") */
	outb(0x0C, CRT_REG_INDEX);

	/* scan line start */
	outb(offset >> 8, CRT_REG_DATA);

	/* CRT index port => demande l'accès au registre 0xa ("addr end") */
	outb(0x0D, CRT_REG_INDEX);

	/* scan line end */
	outb(offset & 0x0000FFFF, CRT_REG_DATA);
}

void switch_page(int i) {
	current_page = i;
	// offset = (@buffer - @base) / nombre d'octets par caractère
	int offset = (((int)pages[i].front_buffer) - BASE_VGA_VIDEO) / sizeof(x86_video_mem);

	offset_video(offset);
}

void flip_page(int n) {
	volatile x86_video_mem *tmp = pages[n].front_buffer;
	pages[n].front_buffer = pages[n].back_buffer;
	pages[n].back_buffer = tmp;
	if (current_page == n)
		switch_page(current_page);
}

static volatile x86_video_mem* get_page(int i, bool front) {
	if (front) {
		return pages[i].front_buffer;
	} else {
		return pages[i].back_buffer;
	}
}

void disable_cursor(int disable) {
	if (disable) {
		/* CRT index port => demande l'accès au registre 0xa ("cursor start") */
		outb(0x0a, CRT_REG_INDEX);

		/* CRT Register 0xa => bit 5 = cursor OFF */
		outb(1 << 5, CRT_REG_DATA);
	} else {

		/* CRT index port => demande l'accès au registre 0xa ("cursor start") */
		outb(0x0a, CRT_REG_INDEX);

		/* scan line start */
		outb(CURSOR_POS_MSB, CRT_REG_DATA);

		/* CRT index port => demande l'accès au registre 0xa ("cursor end") */
		outb(0x0b, CRT_REG_INDEX);

		/* scan line end */
		outb(CURSOR_POS_LSB, CRT_REG_DATA);
	}
}

void cursor_position_video(int n, int x, int y) {
	// position (indice) du caractère = (adresse caractère - adresse base) / (nombre d'octets par caractères)
	int pos = ((int)(pages[n].front_buffer + x + y * COLUMNS) - BASE_VGA_VIDEO) / sizeof(x86_video_mem);

	outb(CURSOR_POS_LSB, CRT_REG_INDEX);
	outb((uint8_t) pos, CRT_REG_DATA);
	outb(CURSOR_POS_MSB, CRT_REG_INDEX);
	outb((uint8_t) (pos >> 8), CRT_REG_DATA);
}

void kputchar_video(int n, bool front, unsigned char c, int x, int y, char attr) {
	if (x < COLUMNS && y < LINES) {
		volatile x86_video_mem *video = get_page(n, front);

		// Conversion latin1 vers cp437.
		c = convert_latin1_cp437[c];

		video[x + y * COLUMNS].character = c & 0xFF;
		video[x + y * COLUMNS].attribute = attr;
	}
}

void scrollup(int n, char attr) {
	volatile x86_video_mem *front = get_page(n, true);
	volatile x86_video_mem *back = get_page(n, false);
	unsigned int l, c;

	for (l = 0; l < LINES - 1; l++) {
		for (c = 0; c < COLUMNS; c++) {
			char car, attr;
			car = front[c + (l + 1) * COLUMNS].character;
			attr = front[c + (l + 1) * COLUMNS].attribute;

			back[c + l * COLUMNS].character = car & 0xFF;
			back[c + l * COLUMNS].attribute = attr;
		}
	}
	for (c = 0; c < COLUMNS; c++) {
		kputchar_video(n, false, ' ', c, LINES - 1, attr);
	}

	flip_page(n);
}
