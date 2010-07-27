//      TACOS: serial.c
//      
//      Copyright 2010 Nicolas Floquet <nicolasfloquet@gmail.com>
//      
//      This program is free software; you can redistribute it and/or modify
//      it under the terms of the GNU General Public License as published by
//      the Free Software Foundation; either version 2 of the License, or
//      (at your option) any later version.
//      
//      This program is distributed in the hope that it will be useful,
//      but WITHOUT ANY WARRANTY; without even the implied warranty of
//      MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//      GNU General Public License for more details.
//      
//      You should have received a copy of the GNU General Public License
//      along with this program; if not, write to the Free Software
//      Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
//      MA 02110-1301, USA.

#include <ioports.h>
#include <serial.h>
#include "serial_masks.h"

#define UART_CLOCK_FREQ	115200

/* Offset des registres */
#define DATA 				0
#define DL_LSB 				0
#define INTERRUPT_ENABLE	1
#define DL_MSB				1
#define	INTERRUPT_ID		2
#define FIFO_CTRL			2
#define	LINE_CTRL			3
#define MODEM_CTRL			4
#define LINE_STATUS			5
#define MODEM_STATUS		6
#define SCRATCH				7

#define write_register(PORT,REG,DATA) outb(DATA,base_addr[PORT]+REG)
#define read_register(PORT,REG) inb(base_addr[PORT]+REG)

/* Adresses de base pour chaque port COM 
 * NOTE: Les ports COM3 et COM4 sont rares, et leurs adresses de base ne sont pas standards 
 */
static const uint32_t base_addr[] = {0x03F8, 0x02F8, 0x03E8, 0x02E8};


/* Initialise le diviseur de fréquence de l'UART pour avoir le bon baud
 * Retourne la fréquence effective (après arrondis)
 */
static int set_baud_rate(serial_port port, unsigned int rate)
{
	int lcr;
	
	uint32_t divisor = 0;
	uint32_t real_rate = 0;
	
	/* On vérifie que la fréquence demandée est réalisable */
	if(rate != 0 && rate <= UART_CLOCK_FREQ)
	{
		divisor = UART_CLOCK_FREQ / rate;
		real_rate = UART_CLOCK_FREQ / divisor;
		
		lcr = read_register(port, LINE_CTRL);
		write_register(port, LINE_CTRL, lcr | DLAB); /* Set DLAB */
		
		write_register(port, DL_LSB, (char)(divisor & 0x0F)); /* LSB */
		write_register(port, DL_MSB, (char)((divisor & 0xF)>>8)); /* MSB */
		
		write_register(port, LINE_CTRL, lcr); /* Clear DLAB */
	}	
	return real_rate;
}

static int set_protocol(serial_port port, char* protocol)
{
	/*****************************************************
	 * Explications sur le registre LINE_CTRL:
	 * Il contrôle le protocol de transfert, c'est à dire le nombre de 
	 * bits par paquet, la présence d'un bit de parité, et le nombre de
	 * bits d'arret.
	 * 
	 * Bits			Rôle
	 * ----------------------------------------- 
	 *	0-1		|	Nombre de bits de données
	 * 			|	0 => 5 bits
	 * 			|	1 => 6 bits
	 * 			|	2 => 7 bits
	 * 			|	3 => 8 bits
	 * -----------------------------------------
	 * 	2		| 	Nombre de bits d'arret
	 * 			|	0 => 1 bit
	 * 			|	1 => 2 bits
	 * ----------------------------------------
	 *	3-5		|	Configure le bit de parité
	 * 			|	XX0 => Aucun		(N)
	 *  		|	001 => Impair		(E)
	 * 			|	011 => Pair			(O)
	 * 			|	101 => Forcé à 1	(M)
	 * 			|	111 => Forcé à 0	(S)
	 * 
	 * Le bit 6 est réservé, et le 7 est le DLAB
	 *
	 * Pour simplifier l'usage, on utilise la syntaxe
	 * standard, et on exprime donc le protocol sous la 
	 * forme "NPS" avec N le nombre de bits de données,
	 * P le type de parité, et S le nombre de bits stop
	 * par exemple:
	 * "8N1" Réfère à 8 bits de données, pas de parité,
	 * et 1 bit stop
	 ******************************************************/
	int ret = -1;
	
	char nb_bits = protocol[0];
	char parity = protocol[1];
	char stop_bit = protocol[2];
	
	char reg_value = 0;
	
	if(nb_bits >= '5' && nb_bits <= '8')
	{
		reg_value = nb_bits - '5';
		ret = 0;
	}
	
	if(ret == 0)
	{
		switch(parity)
		{
			case 'N':
				break; /* 000, autant ne rien faire */
			case 'E':
				reg_value |= EVEN_PARITY;
				break;
			case 'O':
				reg_value |= ODD_PARITY; /* 011 = 3 */
				break;
			case 'M':
				reg_value |= MARK_PARITY; /* 101 = 5 */
				break;
			case 'S':
				reg_value |= SPACE_PARITY; /* 111 = 7 */
				break;
			default:
				ret = -2;
		}
	}
	
	if(ret == 0)
	{
		switch(stop_bit)
		{
			case '1': /* 0, autant ne rien faire */
				break;
			case '2':
				reg_value |= STOP_BIT;
				break;
			default:
				ret = -3;
		}
	}
	kprintf("LCR = 0x%x\n",reg_value);
	if(ret == 0)
		write_register(port, LINE_CTRL, reg_value);
		
	return ret;
}

int serial_init(serial_port port, char* protocol, unsigned int bauds)
{
	int ret = 0;
	
	/* Désactive les interruptions */
	write_register(port, INTERRUPT_ENABLE, 0x00);
		
	if(set_baud_rate(port, bauds) == 0)
		ret = -1;
		
	if(set_protocol(port, protocol) != 0)
		ret = -1;
	
	/* Active la FIFO */
	write_register(port, FIFO_CTRL, FIFO_ENABLE | 
									RCVR_FIFO_RESET | 
									XMIT_FIFO_RESET |
									RCVR_TRIGGER_14);
		
	return ret;
}


static void put_char(serial_port port, char c)
{
	while( !(read_register(port,LINE_STATUS) & THR_EMPTY));
	
	/* Le protocol série impose que le \n soit précédé d'un \r */
	if(c == '\n')
		put_char(port, '\r');
	
	write_register(port, DATA, c);
}

void debug_puts(serial_port port, char* string)
{
	char* ptr = string;
	while(*ptr!=0)
	{
		put_char(port, *ptr);
		ptr++;
	}
}
	
