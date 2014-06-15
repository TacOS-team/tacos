/**
 * @file history.h
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
 * Gestion de l'historique pour readline. Inspiré de l'interface de GNU History.
 */

#ifndef HISTORY_H
#define HISTORY_H


typedef struct _hist_entry {
	char* cmd;
} HIST_ENTRY;

/**
 * Initialise les structures de l'historique.
 */
void using_history();

/**
 * Place la commande à la fin de la liste de l'historique.
 *
 * @param str La commande à inserer dans l'historique.
 */
void add_history(const char *str);

/**
 * Vide l'historique.
 */
void clear_history();

/**
 * Renvoie l'offset courant.
 */
int where_history(void);

/**
 * Fixe la position de l'offset courant.
 * 
 * @param Nouvel offset.
 */
int history_set_pos(int pos);

/**
 * Récupère l'entrée courante.
 */
HIST_ENTRY * current_history(void);

/**
 * Récupère une entrée de l'historique.
 */
HIST_ENTRY* history_get(int offset);

/**
 * Change la position courante vers l'entrée précédente et renvoie un pointeur
 * vers l'entrée de l'historique. Si c'était la première, renvoie NULL.
 */
HIST_ENTRY * previous_history(void);

/**
 * Change la position courante vers l'entrée suivante et renvoie un pointeur
 * vers l'entrée de l'historique. Si c'était la dernière, renvoie NULL.
 */
HIST_ENTRY * next_history(void);

#endif
