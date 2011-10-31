/**
 * @file vga.c
 *
 * @author TacOS developers 
 *
 * @section LICENSE
 *
 * Copyright (C) 2011 - TacOS developers.
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
 * @TODO: "paranoïd" advises from FreeVGA
 * @TODO: currently only supports mode 320*200*256 colors 
 * @TODO: video memory base address is hardcoded
 */
#include <ioports.h>
#include <klog.h>
#include <string.h>
#include <vga.h>

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

static unsigned int VGA_width, VGA_height, VGA_bpp; // Screen width, screen height, bytes per pixel
static unsigned char *VGA_memory; // Base address of VGA memory
static unsigned char font[8192]; // Used to backup the font data when switching to graphic modes
static bool font_saved = FALSE; // Indicates whether the font data can be restored

/**
 * Describes a VGA mode
 */
struct mode {
	int width; /**< Screen width */
	int height; /**< Screen height */
	int bpp; /**< Bytes per pixel (in memory) */
	bool graphic; /**< Text mode (false) or graphic mode (true) */
	unsigned char *reg_values; /**< Values to be written in the registers */
};

static unsigned char mode_320x200x256[] = {
	/* GC */
	0x00, 0x00, 0x00, 0x00, 0x00, 0x40, 0x05, 0x0F,
	0xFF,
	/* SEQ */
	0x03, 0x01, 0x0F, 0x00, 0x0E,
	/* AC */
	0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
	0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F,
	0x41, 0x00, 0x0F, 0x00,	0x00,
	/* CRTC */
	0x5F, 0x4F, 0x50, 0x82, 0x54, 0x80, 0xBF, 0x1F,
	0x00, 0x41, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x9C, 0x0E, 0x8F, 0x28,	0x40, 0x96, 0xB9, 0xA3,
	0xFF,
	/* MISC */
	0x63,
};

static unsigned char mode_320x200x256_modex[] = {
	/* GC */
	0x00, 0x00, 0x00, 0x00, 0x00, 0x40, 0x05, 0x0F,
	0xFF,
	/* SEQ */
	0x03, 0x01, 0x0F, 0x00, 0x06,
	/* AC */
	0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
	0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F,
	0x41, 0x00, 0x0F, 0x00, 0x00,
	/* CRTC */
	0x5F, 0x4F, 0x50, 0x82, 0x54, 0x80, 0xBF, 0x1F,
	0x00, 0x41, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x9C, 0x0E, 0x8F, 0x28, 0x00, 0x96, 0xB9, 0xE3,
	0xFF,
	/* MISC */
	0x63,
};

static unsigned char mode_80x25_text[] = {
	/* GC */
	0x00, 0x00, 0x00, 0x00, 0x00, 0x10, 0x0E, 0x00,
	0xFF,
	/* SEQ */
	0x03, 0x00, 0x03, 0x00, 0x02,
	/* AC */
	0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x14, 0x07,
	0x38, 0x39, 0x3A, 0x3B, 0x3C, 0x3D, 0x3E, 0x3F,
	0x0C, 0x00, 0x0F, 0x08, 0x00,
	/* CRTC */
	0x5F, 0x4F, 0x50, 0x82, 0x55, 0x81, 0xBF, 0x1F,
	0x00, 0x4F, 0x0D, 0x0E, 0x00, 0x00, 0x00, 0x50,
	0x9C, 0x0E, 0x8F, 0x28, 0x1F, 0x96, 0xB9, 0xA3,
	0xFF,
	/* MISC */
	0x67,
};

static struct mode VGA_modes[] = {
	// Text: 80x25
	{ 80, 25, 1, 0, mode_80x25_text },
	// Graphic: 320x200x256 colors
	{ 320, 200, 1, 1, mode_320x200x256 },
	// Graphic: 320x200x256 colors, mode X
	{ 320, 200, 1, 1, mode_320x200x256_modex },
};

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
	inb(other_registers[INSTAT_1_READ]);
}

static void enable_display() {
	/**
	 * @see http://www.osdever.net/FreeVGA/vga/attrreg.htm
	 * PAS -- Palette Address Source (Attribute address register (0x3C0), bit 5)
	 * This bit is set to 0 to load color values to the registers in the internal
	 * palette. It is set to 1 for normal operation of the attribute controller.
	 */
	inb(other_registers[INSTAT_1_READ]);
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

static void backup_font() {
	// The font data is located in plane 2, so we need to set a planar mode
	write_register_values(mode_320x200x256_modex); // XXX
	
	// Tell the Graphics Controller we want to read plane 2
	outb(4, addr_w_registers[GC]);
	outb(2, data_w_registers[GC]);

	// Dump font
	memcpy(font, VGA_memory, 8192);
	
	// Raise a flag indicating that the font has been saved.
	font_saved = TRUE;
}

static void restore_font() {
	// The font data is located in plane 2, so we need to set a planar mode
	write_register_values(mode_320x200x256_modex); // XXX

	// Tell the Graphics Controller we want to write to plane 2
	outb(2, addr_w_registers[SEQ]);
	outb((1 << 2), data_w_registers[SEQ]);

	// Write font
	memcpy(VGA_memory, font, 8192);
}

static void clear_screen() {
	memset(VGA_memory, 0, VGA_width * VGA_height * VGA_bpp);
}

void VGA_write_buf(char *buffer) {
	memcpy(VGA_memory, buffer, VGA_width * VGA_height * VGA_bpp);
}

void VGA_set_mode(enum VGA_mode mode) {
	VGA_width = VGA_modes[mode].width;
	VGA_height = VGA_modes[mode].height;
	VGA_bpp = VGA_modes[mode].bpp;
	VGA_memory = (void*)0xA0000; // TODO: auto-detect
	
	klog("Initializing VGA (%d*%d*%d), base address %x", VGA_width, VGA_height, VGA_bpp, VGA_memory);

	// We need to backup the font data to be able to switch back to text mode
	// later, because it will be overwritten
	if (VGA_modes[mode].graphic) {
		backup_font();
	} else if (font_saved) {
		restore_font();
	}

	write_register_values(VGA_modes[mode].reg_values);

	clear_screen();
}

SYSCALL_HANDLER1(sys_vgasetmode, enum VGA_mode mode) {
	VGA_set_mode(mode);
}

SYSCALL_HANDLER1(sys_vgawritebuf, char *buffer) {
	VGA_write_buf(buffer);
}
