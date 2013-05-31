/**
 * @file floppy_dma.h
 *
 * @author TacOS developers 
 *
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
 * Fonctions liées à l'utilisation du DMA dans le driver disquette
 */

#ifndef _FLOPPY_DMA_H_
#define _FLOPPY_DMA_H_

/**
 * @enum floppy_io
 * @brief Défini le sens de la communication (input / output)
 */
typedef enum {FLOPPY_WRITE = 1, FLOPPY_READ = 2} floppy_io;

/** 
 * @brief Initialise le dma en écriture ou en lecture selon la valeur passée en paramètre
 * @param io_dir Sens de la communication du DMA à initialiser
 */
void floppy_dma_init(floppy_io io_dir);

/**
 * @brief Lis un secteur via le DMA à partir d'un adressage CHS
 * @param cylinder Numero de drive
 * @param cylinder Numero de cylindre
 * @param head Numero de tête de lecture
 * @param sector Numero de secteur
 * @param buffer Buffer recevant les données lues
 */
void floppy_read_sector(int drive, int cylinder, int head, int sector, char* buffer);

/**
 * @brief Ecrit un secteur via le DMA à partir d'un adressage CHS
 * @param cylinder Numero de drive
 * @param cylinder Numero de cylindre
 * @param head Numero de tête de lecture
 * @param sector Numero de secteur
 * @param buffer Buffer contenant les données à écrire
 */
void floppy_write_sector(int drive, int cylinder, int head, int sector, char* buffer);



#endif
