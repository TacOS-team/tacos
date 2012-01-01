/**
 * @file termios.h
 *
 * @author TacOS developers 
 *
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


#ifndef _TERMIOS_H
#define _TERMIOS_H

typedef unsigned int tcflag_t;
typedef unsigned int cc_t;

#define NCCS 17

#define TCGETS 1
#define TCSETS 2

/* c_iflag bits */
#define INLCR 0000100
#define IGNCR 0000200
#define ICRNL 0000400
#define IUCLC 0001000
#define IXON  0002000
#define IXANY 0004000
#define IXOFF 0010000

/* c_oflag bits */
#define OPOST 0000001
#define OLCUC 0000002
#define ONLCR 0000004
#define OCRNL 0000010
#define ONOCR 0000020
#define ONLRET  0000040

/* c_lflag bits */
#define ISIG  0000001
#define ICANON  0000002
#define ECHO  0000010
#define ECHOE 0000020
#define ECHOK 0000040
#define ECHOCTL 0001000
#define ECHOKE  0004000
#define IEXTEN  0100000

/* c_cflag bit meaning */
#define CBAUD 0010017
#define  B0 0000000   /* hang up */
#define  B50  0000001
#define  B75  0000002
#define  B110 0000003
#define  B134 0000004
#define  B150 0000005
#define  B200 0000006
#define  B300 0000007
#define  B600 0000010
#define  B1200  0000011
#define  B1800  0000012
#define  B2400  0000013
#define  B4800  0000014
#define  B9600  0000015
#define  B19200 0000016
#define  B38400 0000017
#define CSIZE 0000060
#define   CS5 0000000
#define   CS6 0000020
#define   CS7 0000040
#define   CS8 0000060
#define CSTOPB  0000100
#define CREAD 0000200
#define PARENB  0000400
#define PARODD  0001000
#define HUPCL 0002000

/* c_cc characters */
#define VINTR 0
#define VQUIT 1
#define VERASE 2
#define VKILL 3
#define VEOF 4
#define VTIME 5
#define VMIN 6
#define VSWTC 7
#define VSTART 8
#define VSTOP 9
#define VSUSP 10
#define VEOL 11
#define VREPRINT 12
#define VDISCARD 13
#define VWERASE 14
#define VLNEXT 15
#define VEOL2 16

struct termios {
	tcflag_t c_iflag;
	tcflag_t c_oflag;
	tcflag_t c_cflag;
	tcflag_t c_lflag;
	cc_t c_cc[NCCS];
	unsigned int c_ispeed;
	unsigned int c_ospeed;
};

int tcgetattr(int fd, struct termios *termios_p);
int tcsetattr(int fd, int optional_actions, const struct termios *termios_p);

#endif
