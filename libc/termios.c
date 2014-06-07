/**
 * @file termios.c
 *
 * @author TacOS developers 
 *
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
 * Description de ce que fait le fichier
 */


#include <termios.h>
#include <sys/ioctl.h>

int tcgetattr(int fd, struct termios *termios_p) {
	ioctl(fd, TCGETS, (void*)termios_p);
	return 0;
}

int tcsetattr(int fd, int optional_actions __attribute__((unused)), const struct termios *termios_p) {
	ioctl(fd, TCSETS, (void*)termios_p);
	return 0;
}

speed_t cfgetispeed(const struct termios *termios_p) {
	return termios_p->c_ispeed;
}

int cfsetispeed(struct termios *termios_p, speed_t speed) {
	termios_p->c_ispeed = speed;
	return 0;
}

void cfmakeraw(struct termios *termios_p) {
//	termios_p->c_iflag &= ~(IGNBRK | BRKINT | PARMRK | ISTRIP
//	                | INLCR | IGNCR | ICRNL | IXON);
//	termios_p->c_oflag &= ~OPOST;
//	termios_p->c_lflag &= ~(ECHO | ECHONL | ICANON | ISIG | IEXTEN);
//	termios_p->c_cflag &= ~(CSIZE | PARENB);
//	termios_p->c_cflag |= CS8;
	termios_p->c_iflag &= ~(INLCR | IGNCR | ICRNL);
	termios_p->c_oflag &= ~OPOST;
	termios_p->c_lflag &= ~(ECHO | ICANON | ISIG);
	termios_p->c_cflag &= ~(CSIZE | PARENB);
	termios_p->c_cflag |= CS8;
}
