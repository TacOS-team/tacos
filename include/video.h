#ifndef _VIDEO_H_
#define _VIDEO_H_

/**
 * @file video.h
 */

#include <types.h>

/** Light blue sur fond noir. */
#define DEFAULT_ATTRIBUTE_VALUE 0x09

#define BLACK         0x0
#define BLUE          0x1
#define GREEN         0x2
#define CYAN          0x3
#define RED           0x4
#define MAGENTA       0x5
#define BROWN         0x6
#define LIGHT_GRAY    0x7
#define DARK_GRAY     0x8
#define LIGHT_BLUE    0x9
#define LIGHT_GREEN   0xA
#define LIGHT_CYAN    0xB
#define LIGHT_RED     0xC
#define LIGHT_MAGENTA 0xD
#define YELLOW        0xE
#define WHITE         0xF

/** The number of columns. */
#define COLUMNS                 80
/** The number of lines. */
#define LINES                   25

/**
 * @brief Structure définissant un "caractère" à l'écran.
 */
typedef struct x86_video_char {
	unsigned char character; /**< Code ASCII du caractère. */
	unsigned char attribute; /**< Attribut qui permet de définir la couleur : [blink(1)|Background(3)|foreground(4)] */
}__attribute__ ((packed)) x86_video_mem;

/**
 * @brief Structure définissant une "page" vidéo. C'est en fait un couple de 2 pages pour gérer le double buffering.
 */
struct vga_page_t {
	volatile x86_video_mem* front_buffer;
	volatile x86_video_mem* back_buffer;
};

/**
 * @brief Initialise l'affichage vidéo. En particulier les 4 pages disponibles.
 */
void init_video();

/**
 * @brief Change la position du curseur.
 */
void cursor_position_video(int x, int y);

/**
 * @brief Désactive l'affichage du curseur.
 */
void disable_cursor(int disable);

/**
 * @brief Interverti les 2 buffers d'une page.
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
 * @brief Active le clignotement du curseur je crois ? De toute façon ça ne marche pas.
 */
void set_blink_bit(int blink_bit);

/**
 * @brief Change la page à afficher à l'écran.
 */
void switch_page(int i);

#endif
