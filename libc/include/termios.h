/**
 * @file termios.h
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


#ifndef _TERMIOS_H
#define _TERMIOS_H

typedef unsigned int tcflag_t;
typedef unsigned int cc_t;
typedef unsigned int speed_t;

#define NCCS 17

#define TCGETS 1
#define TCSETS 2

#define TCSANOW 1 /**< Les modifications sont effectuées immédiatement. */

/* c_iflag bits */
#define INLCR 0000100 /**< Traduit les NL en CR en entrée. */
#define IGNCR 0000200 /**< Ignore les CR. */
#define ICRNL 0000400 /**< Traduit les CR en NL en entrée. */
#define IXON  0002000 /**< Active le XON/XOFF sur le flux de control en sortie. */
#define IXOFF 0010000 /**< Désactive le XON/XOFF sur le flux de control en sortie. */

/* c_oflag bits */
#define OPOST 0000001 /**< Enable implementation-defined output processing. */
#define ONLCR 0000004 /**< Converti les NL en CR sur la sortie. */
#define OCRNL 0000010 /**< Converti les CR en NL sur la sortie. */
#define ONOCR 0000020 /**< Pas de CR sur la colonne 0. */
#define ONLRET 0000040 /**< Pas de CR. */

/* c_lflag bits */
#define ISIG  0000001 /**< Active l'envoi de signaux (INTR, QUIT, SUSP, DSUSP). */
#define ICANON  0000002 /**< Active le mode canonique. */
#define ECHO  0000010 /**< Active l'echoing. */
#define ECHOE 0000020 /**< Si ICANON alors ERASE permet de supprimer. */
#define ECHOK 0000040 /**< Si ICANON alors KILL efface la ligne actuelle. */
#define ECHOCTL 0001000 /**< Si ECHO activé alors ce qui n'est pas un signal de control est affiché sous la forme ^X. */
#define ECHOKE  0004000 /**< Si ICANON le KILL est affiché en effaçant chaque caractère de la ligne. */

/* c_cflag bit meaning */
#define CSIZE 0000060 /**< Masque pour la taille. */
#define   CS5 0000000 /**< 5 bits de données. */ 
#define   CS6 0000020 /**< 6 bits de données. */
#define   CS7 0000040 /**< 7 bits de données. */
#define   CS8 0000060 /**< 8 bits de données. */
#define CSTOPB  0000100 /**< 2 bits de stop au lieu d'un. */
#define CREAD 0000200 /**< Active le receiver. */
#define PARENB  0000400 /**< Bit de parité à la sortie et check en entrée. */
#define PARODD  0001000 /**< Si activé alors parité impaire. */
#define HUPCL 0002000 /**< Lower modem control lines after last process closes the device. */

/* c_cc characters */
#define VINTR 0 /**< Caractère INTR. */
#define VQUIT 1 /**< Caractère QUIT. */
#define VERASE 2 /**< Caractère ERASE. */
#define VKILL 3 /**< Caractère KILL. */
#define VEOF 4 /**< Caractère EOF. */
#define VTIME 5 /**< Time out en noncanonique. */
#define VMIN 6 /**< Nombre minimum de caractère en non canonique. */
#define VSWTC 7 /**< Caractère Switch. */
#define VSTART 8 /**< Caractère Start. */
#define VSTOP 9 /**< Caractère Stop. */
#define VSUSP 10 /**< Caractère Suspend. */
#define VEOL 11 /**< Caractèle EOL. */
#define VREPRINT 12 /**< Reprint unread characters. */
#define VDISCARD 13 /**< Toggle start/stop discarding pending output. */
#define VWERASE 14 /**< Word erase. */
#define VLNEXT 15 /**< Quotes the next input character. */
#define VEOL2 16 /**< Encore un autre EOL. */

struct termios {
	tcflag_t c_iflag; /**< Input modes. */
	tcflag_t c_oflag; /**< Output modes. */
	tcflag_t c_cflag; /**< Control modes. */
	tcflag_t c_lflag; /**< Local modes. */
	cc_t c_cc[NCCS];  /**< Control chars. */
	speed_t c_ispeed; /**< Speed (serial). */
};

/**
 * @brief Récupère la configuration du terminal.
 * 
 * @param fd File descriptor du terminal.
 * @param termios_p Pointeur vers une structure termios pour y placer le
 * résultat.
 *
 * @return 0 en cas de succès, -1 sinon.
 */
int tcgetattr(int fd, struct termios *termios_p);

/**
 * @brief Change la configuration du terminal.
 * 
 * @param fd File descriptor du terminal.
 * @param optional_actions Précise quand les changements doivent avoir lieu (non
 * implémenté).
 * @param termios_p Pointeur vers une structure termios qui contient la
 * configuration.
 *
 * @return 0 en cas de succès, -1 sinon.
 */
int tcsetattr(int fd, int optional_actions, const struct termios *termios_p);

speed_t cfgetispeed(const struct termios *termios_p);
int cfsetispeed(struct termios *termios_p, speed_t speed);
void cfmakeraw(struct termios *termios_p);

#endif
