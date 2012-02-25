/**
 * @file video.h
 *
 * @author TacOS developers 
 *
 * @section LICENSE
 *
 * Copyright (C) 2010, 2011, 2012 - TacOS developers.
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
 * @brief Driver console (vidéo texte). 
 */

#ifndef _VIDEO_H_
#define _VIDEO_H_

#include <types.h>

/** Light blue sur fond noir. */
#define DEFAULT_ATTRIBUTE_VALUE 0x0F /**< Attribut par défaut d'un caractère */

#define BLACK         0x0   /**< Code couleur noir. */
#define BLUE          0x1   /**< Code couleur bleu. */
#define GREEN         0x2   /**< Code couleur vert. */
#define CYAN          0x3   /**< Code couleur cyan. */
#define RED           0x4   /**< Code couleur rouge. */
#define MAGENTA       0x5   /**< Code couleur magenta. */
#define BROWN         0x6   /**< Code couleur marron. */
#define LIGHT_GRAY    0x7   /**< Code couleur gris clair. */
#define DARK_GRAY     0x8   /**< Code couleur gris sombre. */
#define LIGHT_BLUE    0x9   /**< Code couleur bleu clair. */
#define LIGHT_GREEN   0xA   /**< Code couleur vert clair. */
#define LIGHT_CYAN    0xB   /**< Code couleur cyan clair. */
#define LIGHT_RED     0xC   /**< Code couleur rouge clair. */
#define LIGHT_MAGENTA 0xD   /**< Code couleur magenta clair. */
#define YELLOW        0xE   /**< Code couleur jaune. */
#define WHITE         0xF   /**< Code couleur blanc. */

#define COLUMNS                 80 /**< The number of columns. */
#define LINES                   25 /** The number of lines. */

/**
 * @brief Structure définissant un "caractère" à l'écran.
 */
typedef struct {
	unsigned char character; /**< Code ASCII du caractère. */
	unsigned char attribute; /**< Attribut qui permet de définir la couleur : [blink(1)|Background(3)|foreground(4)] */
}__attribute__ ((packed)) x86_video_mem;

/**
 * @brief Structure définissant une "page" vidéo. C'est en fait un couple de 2 pages pour gérer le double buffering.
 */
struct vga_page_t {
	volatile x86_video_mem* front_buffer; /**< Adresse du buffer au premier plan. */
	volatile x86_video_mem* back_buffer;  /**< Adresse du buffer au second plan. */
};

/**
 * @brief Initialise l'affichage vidéo. En particulier les 4 pages disponibles.
 */
void init_video();

/**
 * @brief Change la position du curseur.
 */
void cursor_position_video(int n, int x, int y);

/**
 * @brief Désactive l'affichage du curseur.
 *
 * @param disable 1 si le curseur ne doit pas être affiché. 0 sinon.
 */
void disable_cursor(int disable);

/**
 * @brief Interverti les 2 buffers d'une page.
 *
 * @param n Le numero de la page concernée.
 */
void flip_page(int n);

/**
 * @brief Permet de récupérer le caractère situé à une certaine position de l'écran.
 */
void get_char_video(int n, bool front, char *c, int x, int y, char *attr);

/**
 * @brief Affiche dans une page le caractère souhaité à l'endroit souhaité.
 */
void kputchar_video(int n, bool front, char c, int x, int y, char attr);

/**
 * @brief Active le clignotement du curseur.
 *
 * @param blink_bit 1 si le cursor doit clignoter.
 */
//void set_blink_bit(int blink_bit);

/**
 * @brief Change la page à afficher à l'écran.
 *
 * @param i Le numero de la page concernée.
 */
void switch_page(int i);

#endif
