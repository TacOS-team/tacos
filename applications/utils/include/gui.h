/**
 * @file gui.h
 *
 * @author TacOS developers 
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
 * API minimaliste pour faire des interfaces graphiques en mode texte
 */

#ifndef _GUI_H_
#define _GUI_H_

#include <sys/types.h>

/** 
* @brief défini une "fenêtre", il s'agit d'une instance de gui pour l'application
*/
struct window_t;
/** 
* @brief défini un "widget" il s'agit d'un element d'interface, une fenêtre est composée de plusieurs widgets
*/
struct widget_t;

/**
*** Manipulation des fenêtres
**/

/** 
* @brief crée une fenêtre
* 
* @param bg la couleur du fond
* @param cursor la couleur du curseur
* 
* @return un pointeur vers la fenêtre
*/
struct window_t* createWindow(unsigned char bg, unsigned char cursor);
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
void freeWindow(struct window_t* win);

/**
*** Manipulation des widgets
**/

/** 
* @brief ajoute un widget de type bouton à une fenêtre
* 
* @param win la fenêtre qui contiendra le bouton
* @param title le texte qui s'affichera sur le bouton (une seule ligne)
* 
* @return un pointeur vers le widget
*/
struct widget_t* addButton(struct window_t* win, const char* title);
void setText(struct widget_t* wdg, const char* text);

/** 
* @brief ajoute un widget de type champ de texte à une fenêtre
* 
* @param win la fenêtre qui contiendra le champ
* @param txt le texte que contiendra le champ, celui ci peut inclure des retours à la ligne
* 
* @return 
*/
struct widget_t* addTxt(struct window_t* win, const char* txt);
/** 
* @brief modifie les propriétés d'un widget
* 
* @param wdg le widget à modifier
* @param x la position x
* @param y la position y
* @param h la hauteur "height" du widget
* @param w la largeur "width" du widget
* @param bg la couleur de fond du widget
* @param fg la couleur de police du widget
*/
void setWidgetProperties(struct widget_t* wdg, 
									unsigned char x, unsigned char y,
									unsigned char h, unsigned char w,
									unsigned char bg, unsigned char fg);
/** 
* @brief permet de mettre un handler qui sera déclenché des que l'on clique sur le widget
* 
* @param wdg le widget qui deviendra sensible aux clics
* @param onClick la fonction à appeler lorsque l'on clique sur le widget
*/
void setOnClick(struct widget_t* wdg, void (*onClick)(struct widget_t*,int,int));
void setVisible(struct widget_t* wdg, char visible);

#endif //_GUI_H_

