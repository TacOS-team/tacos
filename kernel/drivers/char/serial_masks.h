/**
 * @file serial_masks.h
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


#ifndef _SERIAL_MASKS_H_
#define _SERIAL_MASKS_H_

/* Interrupt Enable Register */
#define ERBFI	0x01	/* Enable Receive Data Available Interrupt */
#define ETBEI	0x02	/* Enable Transmit Holding Register Empty Interrupt */
#define ELSI	0x04	/* Enable Receiver Line Status Interrupt */
#define EDSSI	0x08	/* Enable MODEM Status Interrupt */

/* Interrupt identification register */
#define INTERRUPT_PENDING		0x01
#define INTERRUPT_ID(reg)		(reg&0x0F)
/* #define	FIFO_ENABLED(reg)		(reg&0xC0)>>6 */

#define INT_NONE	0x1
#define INT_RX_LINE_STATUS	0x6
#define	INT_DATA_AVAILABLE	0x4
#define	INT_CHAR_TIMEOUT	0xC
#define	INT_THR_EMPTY		0x2
#define	INT_MODEM_STATUS	0x0

/* FIFO Control Register */
#define FIFO_ENABLE		0x01
#define	RCVR_FIFO_RESET	0x02
#define XMIT_FIFO_RESET	0x04
#define DMA_MODE_SELECT	0x08

#define RCVR_TRIGGER	0xC0
#define RCVR_TRIGGER_1	0x00
#define RCVR_TRIGGER_4	0x40
#define RCVR_TRIGGER_8	0x80
#define RCVR_TRIGGER_14	0xC0

/* Line Control Register */
#define WORD_LENGTH	0x03
#define STOP_BIT	0x04

#define PARITY_BIT		0x38
#define NO_PARITY		0x00
#define EVEN_PARITY		0x08
#define ODD_PARITY		0x18
#define MARK_PARITY		0x28
#define SPACE_PARITY	0x38

#define SET_BREAK	0x40
#define DLAB		0x80

/* MODEM Control Register */
#define DTR		0x01
#define RTS		0x02
#define OUT1	0x04
#define OUT2	0x08
#define LOOP	0x10

/* Line Status Register */
#define DATA_READY		0x01
#define OVERRUN_ERROR	0x02
#define PARITY_ERROR	0x04
#define FRAMING_ERROR	0x08
#define BREAK_INTERRUPT	0x10
#define	THR_EMPTY		0x20
#define TX_EMPTY		0x40
#define	RCVR_FIFO_ERROR	0x80

/* MODEM Status Register */
#define DCTS	0x01
#define DDSR	0x02
#define TERI	0x04
#define DDCD	0x08
#define CTS		0x10
#define DSR		0x20
#define RI		0x40
#define DCD		0x80

#endif /* _SERIAL_MASKS_H_ */
