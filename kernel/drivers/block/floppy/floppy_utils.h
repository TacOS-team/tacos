/**
 * @file floppy_utils.h
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
 * Description de ce que fait le fichier
 */

#ifndef _FLOPPY_UTILS_H_
#define _FLOPPY_UTILS_H_

#define FLOPPY_BASE	0x03f0

enum floppy_registers
{
	FLOPPY_SRA = 0, // Status register A		-- read-only
	FLOPPY_SRB = 1, // Status register B		-- read-only
	FLOPPY_DOR = 2, // Digital output register
	FLOPPY_TDR = 3, // Tape drive register
	FLOPPY_MSR = 4, // Main status register		-- read-only
	FLOPPY_DSR = 4, // Datarate select register	-- write-only
	FLOPPY_FIFO = 5, // Data FIFO
	FLOPPY_DIR = 7, // Digital input register	-- read-only
	FLOPPY_CCR = 7  // Configuration control register -- write-only
};

enum floppy_commands
{
	READ_TRACK =			2,
	SPECIFY =				3,
	SENSE_DRIVE_STATUS =	4,
	WRITE_DATA = 			5,
	READ_DATA =				6,
	RECALIBRATE = 			7,
	SENSE_INTERRUPT =		8,
	READ_ID =				10,
	READ_DELETED_DATA =		12,
	FORMAT_TRACK =			13,
	SEEK =					15,
	VERSION =				16,
	SCAN_EQUAL =			17,
	PERPENDICULAR_MODE =	18,
	CONFIGURE =				19,
	LOCK = 					20,
	VERIFY =				22,
	SCAN_LOW_OR_EQUAL =		25,
	SCAN_HIGH_OR_EQUAL =	29
};


bool floppy_ready();
int floppy_write_command(char cmd);
uint8_t floppy_read_data();

// Drive selection
uint8_t floppy_get_current_drive();
void floppy_set_current_drive(uint8_t drive);

int floppy_seek(int cylindre, int head);

/*	
 * MISC  
 */
uint8_t floppy_get_type(int drive);
void floppy_detect_drives();
uint8_t floppy_get_version();

#endif
