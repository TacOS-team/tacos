/**
 * @file vga.c
 *
 * @author TacOS developers 
 *
 * @section LICENSE
 *
 * Copyright (C) 2011, 2012, 2013 - TacOS developers.
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
 * VGA driver.
 *
 * @see http://wiki.osdev.org/VGA_Hardware
 * @see http://www.osdever.net/FreeVGA/vga/vga.htm
 * @see http://files.osdev.org/mirrors/geezer/osd/graphics/modes.c
 * (modes structs taken from there)
 * TODO: "paranoïd" advises from FreeVGA
 * TODO: currently only supports mode 320*200*256 colors 
 * TODO: video memory base address is hardcoded
 */
#include <fs/devfs.h>
#include <ioports.h>
#include <klog.h>
#include <klibc/string.h>
#include <drivers/vga.h>
#include <drivers/vga_modes.h>

/**
 * Defines the number of registers and the ports corresponding to the address
 * registers (addr_w_registers) and the data read/write registers
 * (data_r/w_registers) for the 5 groups of VGA registers.
 * @see http://www.osdever.net/FreeVGA/vga/vga.htm
 *   | GC: Graphics Registers
 *   -- control the way the CPU accesses video RAM.
 *   | SEQ: Sequencer Registers
 *   -- control how video data is sent to the DAC.
 *   | AC:  Attribute Controller Registers
 *   -- selects the 16 color and 64 color palettes used for EGA/CGA compatibility.
 *   | CRTC: CRT Controller Registers
 *   -- control how the video is output to the display.
 *   | MISC: External Registers
 *   -- miscellaneous registers used to control video operation.
 */
#define NB_REGISTER_GROUPS 5
enum reg_groups_names             {      GC,     SEQ,      AC,    CRTC,    MISC   };
static short addr_w_registers[] = {   0x3CE,   0x3C4,   0x3C0,   0x3D4,   0x000   };
static short data_w_registers[] = {   0x3CF,   0x3C5,   0x3C0,   0x3D5,   0x3C2   };
static short data_r_registers[] = {   0x3CF,   0x3C5,   0x3C1,   0x3D5,   0x3CC   }; 
static int nb_registers[] =       {       9,       5,      21,      25,       1   };

/**
 * Defines the ports corresponding to other registers used.
 *    | INSTAT_1_READ: Input Status #1 Register
 *    -- Used to initialize the Attribute controllers registers flipflop
 *       (@see reset_ac_controllers_flipflop)
 */
enum other_regs_names             {   INSTAT_1_READ   };
static short other_registers[] =  {           0x3DA   };

static unsigned int vga_width, vga_height, vga_bpp; // Screen width, screen height, bytes per pixel
static bool graphic_mode = false;
static unsigned char *vga_memory; // Base address of VGA memory
static unsigned char font[8192]; // Used to backup the font data when switching to graphic modes
static unsigned char backup_text_regs[64];
//static unsigned char text_ram[0x02000];
static bool font_saved = false; // Indicates whether the font data can be restored

static void unlock_crtc_registers() {
	/**
	 * @see http://www.osdever.net/FreeVGA/vga/vgareg.htm 
	 * Note that certain CRTC registers can be protected from read or write access
	 * for compatibility with programs written prior to the VGA's existence.
	 * This protection is controlled via the Enable Vertical Retrace Access
	 * (idx 0x3, b7) and CRTC Registers Protect Enable fields (idx 0x11, b7)
	 */
	outb(0x03, addr_w_registers[CRTC]);
	outb(inb(data_r_registers[CRTC]) | (1 << 7), data_w_registers[CRTC]);
	outb(0x11, addr_w_registers[CRTC]);
	outb(inb(data_r_registers[CRTC]) & ~(1 << 7), data_w_registers[CRTC]);
}

static void reset_ac_registers_flipflop() {
	/**
	 * @see http://www.osdever.net/FreeVGA/vga/vgareg.htm
	 * The attribute registers are also accessed in an indexed fashion [...].
	 * The index and the data are written to the same port, one after another.
	 * A flip-flop inside the card keeps track of whether the next write will be
	 * handled is an index or data. Because there is no standard method of
	 * determining the state of this flip-flop, the ability to reset the flip-flop
	 * such that the next write will be handled as an index is provided.
	 * This is accomplished by reading the Input Status #1 Register [...].
	 */
	(void) inb(other_registers[INSTAT_1_READ]);
}

static void enable_display() {
	/**
	 * @see http://www.osdever.net/FreeVGA/vga/attrreg.htm
	 * PAS -- Palette Address Source (Attribute address register (0x3C0), bit 5)
	 * This bit is set to 0 to load color values to the registers in the internal
	 * palette. It is set to 1 for normal operation of the attribute controller.
	 */
	(void) inb(other_registers[INSTAT_1_READ]);
	outb(1 << 5, addr_w_registers[AC]);
}

static void write_register_values(unsigned char *values) {
	// CRTC register might be locked, unlock them
	unlock_crtc_registers();
	// Set the initial value of the Attribute Controller registers flipflop
	reset_ac_registers_flipflop();

	int group, index;
	// For each group of registers (GC, SEQ, AC, CRTC, MISC)...
	for (group = 0; group < NB_REGISTER_GROUPS; group++) {
		// For each register in this group
		for (index = 0; index < nb_registers[group]; index++, values++) {
			// Select the register to write by writing its index to the address register, if needed
			if (addr_w_registers[group] > 0) {
				outb(index, addr_w_registers[group]);
			}
			// Set the value by writing to the data register
			outb(*values, data_w_registers[group]);
		}
	}

	// Finally, enable the display by locking the palette
	enable_display();
}

static void read_register_values(unsigned char *values) {
	int group, index;
	// For each group of registers (GC, SEQ, AC, CRTC, MISC)...
	for (group = 0; group < NB_REGISTER_GROUPS; group++) {
		// For each register in this group
		for (index = 0; index < nb_registers[group]; index++, values++) {
			// Select the register to read by writing its index to the address register, if needed
			if (addr_w_registers[group] > 0) {
				if (group == AC) {
					reset_ac_registers_flipflop();
				}
				outb(index, addr_w_registers[group]);
			}
			// Read the data register
			*values = inb(data_r_registers[group]);
		}
	}

	enable_display();
}

static void backup_font_data() {
	// The font data is located in plane 2, so we need to set a planar mode
	write_register_values(mode_320x200x256_modex); // XXX
	
	// Tell the Graphics Controller we want to read plane 2
	outb(4, addr_w_registers[GC]);
	outb(2, data_w_registers[GC]);

	// Dump font
	memcpy(font, vga_memory, 8192);
	
	// Raise a flag indicating that the font has been saved.
	font_saved = true;
}

static void restore_font_data() {
	// The font data is located in plane 2, so we need to set a planar mode
	write_register_values(mode_320x200x256_modex); // XXX

	// Tell the Graphics Controller we want to write to plane 2
	outb(2, addr_w_registers[SEQ]);
	outb((1 << 2), data_w_registers[SEQ]);

	// Write font
	memcpy(vga_memory, font, 8192);
}

static void clear_screen() {
	memset(vga_memory, 0, vga_width * vga_height * vga_bpp);
}

void vga_write_buf(char *buffer) {
	memcpy(vga_memory, buffer, vga_width * vga_height * vga_bpp);
}

void vga_set_mode(enum vga_mode mode) {
	vga_width = vga_modes[mode].width;
	vga_height = vga_modes[mode].height;
	vga_bpp = vga_modes[mode].bpp;
	vga_memory = (void*)0xA0000; // TODO: auto-detect
	
	klog("Initializing vga (%d*%d*%d), base address %x", vga_width, vga_height, vga_bpp, vga_memory);

	// We need to backup the font data to be able to switch back to text mode
	// later, because it will be overwritten
	if (!graphic_mode && vga_modes[mode].graphic) {
		read_register_values(backup_text_regs);
		backup_font_data();
	}

	write_register_values(vga_modes[mode].reg_values);

	clear_screen();
}

static void vga_back_to_text_mode() {
	restore_font_data();
	write_register_values(backup_text_regs);
}

static int vga_ioctl(open_file_descriptor* ofd __attribute__ ((unused)), unsigned int request, void* data) {
	switch (request) {
		case SETMODE: {
			enum vga_mode mode = (enum vga_mode)data;
			// TODO: check mode
			vga_set_mode(mode);
			return 0;
		}
		case FLUSH: {
			char *user_buffer = (char*)data;
			vga_write_buf(user_buffer);				
			return 0;
		}
		case BACKTOTEXTMODE: {
			vga_back_to_text_mode();
			return 0;
		}
		default:
			return -1;
	}
} 

static chardev_interfaces di = {
	.read = NULL,
	.write = NULL,
	.open = NULL,
	.close = NULL,
	.ioctl = vga_ioctl,
};

void init_vga() {
	klog("initializating vga driver...");

	if (register_chardev("vga", &di) != 0) {
		kerr("error registering vga driver.");
	}
}
