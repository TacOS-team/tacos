/**
 * @file mouse.h
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

#ifndef _MOUSE_H_
#define _MOUSE_H_

/** 
* @file mouse.h
* @brief driver souris
*/

/** 
* @brief initialisation du driver souris
*
* Initialisation de la souris
* Mise en place du handler d'interruption
*
*/
void mouseInit();

/** 
* @brief handler d'interruption du driver souris
* 
* met à jour les coordonnées de la souris
* met à jour l'etat des boutons
*
* @param id non utilisé
*
* @todo si on devait gérer des callbacks pour la souris ca se ferait ici
*/
void mouseInterrupt(int id);

/** 
* @brief premet de recuperer les coordonnées de la souris
* 
* @param x pointeur vers l'entier qui contiendra x
* @param y pointeur vers l'entier qui contiendra y
*/
void getMouseCoord(int* x, int* y);

/** 
* @brief permet d'obtenir l'etat d'un bouton
* 
* @param btnId l'identifiant du bouton 
* 
* @return un entier 0 si le bouton est relaché, 1 si il est appuyé
*/
int getMouseBtn(int btnId);

#endif

