/**
 * @file serial.c
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

/* TODO: Suggestions de modifications:
 * 1) Changer le comportement de serial_puts: là si le buffer est plein on arrete d'envoyer, ce n'est peu être pas idéal...
 * 2) Ajuster la taille des buffer : Arrive-t-il qu'ils soient plein? trop souvent? jamais?
 * 3) Améliorer la manière d'avoir toujours assez de place pour inserer le \r avant le \n (cf. macro TX_BUFFER_FULL)
 * */

#include <interrupts.h>
#include <ioports.h>
#include <drivers/serial.h>
#include <fs/devfs.h>
#include <klog.h>
#include <ksem.h>
#include <kfcntl.h>
#include <kmalloc.h>
#include <tty.h>

#include "serial_masks.h"

#define RX_BUFFER_SIZE	256
#define TX_BUFFER_SIZE	256
#define TX_FIFO_SIZE	16 /* D'après la datasheet du PC1550D, pas évident que ce soit le bon model */

#define UART_CLOCK_FREQ	115200

/* Offset des registres */
#define DATA 				0
#define DL_LSB 				0
#define INTERRUPT_ENABLE	1
#define DL_MSB				1
#define	INTERRUPT_ID_REG	2
#define FIFO_CTRL			2
#define	LINE_CTRL			3
#define MODEM_CTRL			4
#define LINE_STATUS			5
#define MODEM_STATUS		6
#define SCRATCH				7

#define write_register(PORT,REG,DATA) outb(DATA,base_addr[PORT]+REG)
#define read_register(PORT,REG) inb(base_addr[PORT]+REG)

/***************************************
 * 
 * PROTOTYPES
 *
 **************************************/
 
static int set_baud_rate(serial_port port, unsigned int rate);
static int set_protocol(serial_port port, char* protocol);

/**************************************
 * 
 * DECLARATIONS GLOBALES
 * 
 **************************************/

/* Adresses de base pour chaque port COM 
 * NOTE: Les ports COM3 et COM4 sont rares, et leurs adresses de base ne sont pas standards 
 */
static const uint32_t base_addr[] = {0x03F8, 0x02F8, 0x03E8, 0x02E8};
static uint8_t	flags_array[] = { 0, 0, 0, 0 };

static tty_driver_t *tty_driver;

/* Buffers de communication:
 * On va s'en servir comme buffer circulaire, il faut donc un pointeur de début et de fin pour chacun
 */

static char tx_buffer[4][TX_BUFFER_SIZE];
static int tx_start[4];
static int tx_end[4];
static int tx_size[4];

/* Petit hack pour permettre d'avoir toujours la place pour inserer un \r avant le \n */
#define TX_BUFFER_FULL(port) ((tx_size[port] == TX_BUFFER_SIZE-1)?1:0)


/**************************************
 * 
 * FONCTIONS
 * 
 **************************************/



static void put_char(serial_port port, char c)
{
	while( !(read_register(port,LINE_STATUS) & THR_EMPTY));
	write_register(port, DATA, c);
}

static void serial_putc(tty_struct_t *tty, const unsigned char c)
{
	int port = tty->index;
	char ier = read_register(port, INTERRUPT_ENABLE);
	
	if(!TX_BUFFER_FULL(port)) //XXX: remplacer par une sémaphore et attendre.
	{
		if(c == '\n')
		{
			tx_buffer[port][tx_end[port]] = '\r';
			
			tx_size[port]++;
			tx_end[port] = (tx_end[port]+1)%TX_BUFFER_SIZE;
		}
				
		tx_buffer[port][tx_end[port]] = c;

		tx_size[port]++;
		tx_end[port] = (tx_end[port]+1)%TX_BUFFER_SIZE;	
	}
	
	/* On active l'interruption de transmission si c'est pas déja le cas */
	if(!(ier & ETBEI))
	{
		ier |= ETBEI;
		write_register(port, INTERRUPT_ENABLE, ier);
	}
}
	
size_t serial_write(tty_struct_t *tty, open_file_descriptor* odf __attribute__((unused)), const unsigned char* buf, size_t count)
{
	unsigned int i = 0;
	char* ptr = (char*) buf;

	while(*ptr!=0 && i<count)
	{
		serial_putc(tty, *ptr);
		i++;
		ptr++;
	}
	return i;
}

void serial_isr(int id __attribute__ ((unused)))
{
	int interrupt_identifier = 0;
	int i_id;
	char temp_read;
	int i;
	int counter;
	
	/* On traite l'interruption pour chaque port com initialisé */
	for(i = 0; i<4; i++)
	{
		if(flags_array[i]& 0x1)
		{
				interrupt_identifier = read_register(i, INTERRUPT_ID_REG);

				
				/* Si une interruption a été levée sur ce port, on la traite */
				if(interrupt_identifier & INTERRUPT_PENDING || 1)
				{
					i_id = INTERRUPT_ID(interrupt_identifier);
					switch(i_id)
					{	
							case INT_NONE:
								kerr("None interrupt");
								break;
							case INT_RX_LINE_STATUS:
								temp_read = read_register(i, LINE_STATUS);
								kerr("Line status interrupt");
								break;
							case INT_DATA_AVAILABLE:			/* Que ce soit en data available ou en timeout, il faut récupérer les données dans le buffer si possible */
								kerr("Data received.");
							case INT_CHAR_TIMEOUT:
								while(read_register(i, LINE_STATUS) & DATA_READY)
								{
									temp_read = read_register(i, DATA);
									tty_insert_flip_char(tty_driver->ttys[i], temp_read);
								}
								break;
							case INT_THR_EMPTY:
								
								/* Si on a des choses à envoyer, on les envoit */
								if(tx_size[i]>0)
								{
									kdebug("TX=>send");
									counter = 0;
									while(counter < TX_FIFO_SIZE && tx_size[i]>0)
									{
										
										put_char(i, tx_buffer[i][tx_start[i]]);	
										tx_size[i]--;
										tx_start[i] = (tx_start[i]+1)%TX_BUFFER_SIZE;
										counter++;
									}
								}
								else /* Sinon on désactive l'interruption de transmission */
								{
									kdebug("TX=>closing");
									temp_read = read_register(i, INTERRUPT_ENABLE);
									temp_read &= (~ETBEI);
									write_register(i, INTERRUPT_ENABLE, temp_read);
								}
								break;
							case INT_MODEM_STATUS:
								kerr("Modem status");
								break;
							default:
								kerr("Unknown interrupt");
								break;
								
						}
					}
			}
	}
}

/* Initialise le diviseur de fréquence de l'UART pour avoir le bon baud
 * Retourne la fréquence effective (après arrondis)
 */
static int set_baud_rate(serial_port port, unsigned int rate)
{
	int lcr;
	
	uint32_t divisor = 0;
	uint32_t real_rate = 0;
	
	kdebug("set_baud_rate");
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
	
	kdebug("set_set_protocol");
	
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
	
	if(ret == 0)
		write_register(port, LINE_CTRL, reg_value);
		
	return ret;
}

int serial_init()
{
	char* protocol = "8N1";
	unsigned int bauds = 9600;
	int ret = 0;
	
	//klog("Initialisation du port série...");
	
	int port;
	for (port = 0; port < 4; port++) {
		/* Désactive les interruptions */
		write_register(port, INTERRUPT_ENABLE, 0x00);
			
		/* Configuration du controleur */ 
		if(set_baud_rate(port, bauds) == 0)
			ret = -1;
			
		if(set_protocol(port, protocol) != 0)
			ret = -1;
		
		/* Active la FIFO */
		write_register(port, FIFO_CTRL, FIFO_ENABLE 	| 
										RCVR_FIFO_RESET | 
										XMIT_FIFO_RESET |
										RCVR_TRIGGER_14);
										
		/* On note quelque part qu'on a bien initialisé le controleur */
		flags_array[port] = ret?0:1;
		
		/* On initialise les buffers du port */
		tx_start[port] = 0;
		tx_end[port] = 0; 
		tx_size[port] = 0;
		
		/* Active les interruption (DEBUG) 
		 * Note: A l'initialisation on n'active que l'interruption de 
		 * réception, celle de transmission sera activée quand on voudra
		 * envoyer des données.
		 */
		write_register(port, INTERRUPT_ENABLE,	ERBFI); /* Cf. serial_mask.h */
	}
	
	/* Enregistre le driver */
	tty_driver = alloc_tty_driver(4);
	tty_driver->driver_name = "serial";
	tty_driver->devfs_name = "ttyS";
	tty_driver->type = TTY_DRIVER_TYPE_SERIAL;
	tty_driver->init_termios = tty_std_termios;
	tty_driver->ops = kmalloc(sizeof(tty_operations_t));
	tty_driver->ops->open = NULL;
	tty_driver->ops->close = NULL;
	tty_driver->ops->write = serial_write;
	tty_driver->ops->put_char = serial_putc;
	tty_driver->ops->set_termios = NULL;
	tty_driver->ops->ioctl = NULL;
	tty_register_driver(tty_driver);
	
	return ret;
}
	
