/**
 * @file errno.c
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
 * @brief Contient la variable d'erreur errno.
 */

int errno;

const char* sys_errlist[] = {
"Erreur inconnue ou pas d'erreur.",
"Essai encore ;).",
"Mauvais numéro de fichier.",
"Mauvaise adresse.",
"Fichier trop gros.",
"System call interrompu.",
"Argument invalide.",
"Erreur d'entrée / sortie.",
"Plus d'espace libre sur le périphérique.",
"Tube brisé.",
"Résultat mathématique non representable.",
"Entrée non trouvée.",
"Ce n'est pas un dossier.",
"Le fichier existe déjà.",
"Opération non permise.",
"C'est un dossier.",
"Overflow de la file table.",
"Trop de fichiers ouverts."};

