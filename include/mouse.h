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

