#ifndef GUI_H
#define GUI_H

#include <types.h>

/** 
* @brief défini une "fenêtre", il s'agit d'une instance de gui pour l'application
*/
struct window_t;
/** 
* @brief défini un "widget" il s'agit d'un element d'interface, une fenêtre est composée de plusieurs widgets
*/
struct widget_t;

/** 
* @brief crée une fenêtre
* 
* @param bg la couleur du fond
* @param cursor la couleur du curseur
* 
* @return un pointeur vers la fenêtre
*/
struct window_t* createWindow(uint8_t bg, uint8_t cursor);
/** 
* @brief lance la boucle principale de la fenêtre (mise à jour de la souris, gestion des clics ...)
* 
* @param win
*/
void runWindow(struct window_t* win);
/** 
* @brief détruit la fenêtre, libère la mémoire
* 
* @param win la fenêtre à détruire
* 
* @return 0 en cas de succès
*/
int freeWindow(struct window_t* win);

struct widget_t* addButton(struct window_t* win, const char* title);
struct widget_t* addTxt(struct window_t* win, const char* txt);
void setWidgetProperties(struct widget_t* wdg, 
									uint8_t x, uint8_t y,
									uint8_t h, uint8_t w,
									uint8_t bg, uint8_t fg);
void setOnClick(struct widget_t* wdg, void (*onClick)(struct widget_t*,int,int));

#endif //GUI_H

