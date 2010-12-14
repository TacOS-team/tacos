/**
 * @file rtl8139.c
 *
 * @author TacOS developers 
 *
 * Maxime Cheramy <maxime81@gmail.com>
 * Nicolas Floquet <nicolasfloquet@gmail.com>
 * Benjamin Hautbois <bhautboi@gmail.com>
 * Ludovic Rigal <ludovic.rigal@gmail.com>
 * Simon Vernhes <simon@vernhes.eu>
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

#include <pci.h>
#include <pci_config.h>
#include <ioports.h>
#include <string.h>
#include <kstdio.h>
#include <klog.h>

#define RTL8139_VENDOR_ID	0x10EC	/* Realtek Semiconductor’s PCI Vendor ID */
#define RTL8139_DEVICE_ID	0x8139	/* RTL8139 PCI Device ID */

/* I/O Space registers */
#define	IDR_BASE	0x00
#define	IDR0	0x00
#define	IDR1	0x01
#define	IDR2	0x02
#define	IDR3	0x03
#define	IDR4	0x04
#define	IDR5	0x05
#define	MAR		0x08
#define	TSD_BASE	0x10
#define TSD0	0x10
#define TSD1	0x14
#define TSD2	0x18
#define TSD3	0x1C
#define	TSAD_BASE	0x20
#define	TSAD0	0x20
#define	TSAD1	0x24
#define	TSAD2	0x28
#define	TSAD3	0x2C
#define	RBSTART	0x30
#define	ERBCR	0x34
#define	ERSR	0x36
#define	CR		0x37
#define	CAPR	0x38
#define	CBR		0x3A
#define	IMR		0x3C
#define	ISR		0x3E
#define	TCR		0x40
#define	RCR		0x44
#define	TCTR	0x48
#define	MPC		0x4C
#define	R9346C	0x50
#define	CONFIG0	0x51
#define	CONFIG1 0x52
#define	TimerInt	0x54
#define	MSR		0x58
#define	CONFIG3	0x59
#define	CONFIG4	0x5A
#define	MULINT	0x5C	
#define	RERID	0x5E
#define	TSAD	0x60
#define	BMCR	0x62
#define	BMSR	0x64
#define	ANAR	0x66
#define	ANLPAR	0x68
#define	ANER	0x6A
#define	DIS		0x6C
#define	FCSC	0x6E
#define	NWAYTR	0x70
#define	REC		0x72
#define	CSCR	0x74
#define	PHY1_PARM	0x78
#define	TW_PARM	0x7C
#define	PHY2_PARM	0x80
#define	CRC_BASE	0x84	
#define	CRC0	0x84
#define	CRC1	0x85
#define	CRC2	0x86
#define	CRC3	0x87
#define	CRC4	0x88
#define	CRC5	0x89
#define	CRC6	0x8A
#define	CRC7	0x8B
#define Wakeup_BASE	0x8C
#define Wakeup0	0x8C
#define Wakeup1	0x94
#define Wakeup2	0x9C
#define Wakeup3	0xA4
#define Wakeup4	0xAC
#define Wakeup5	0xB4
#define Wakeup6	0xBC
#define Wakeup7	0xC4
#define LSBCRC_BASE	0xCC
#define LSBCRC0	0xCC
#define LSBCRC1	0xCD
#define LSBCRC2	0xCE
#define LSBCRC3	0xCF
#define LSBCRC4	0xD0
#define LSBCRC5	0xD1
#define LSBCRC6	0xD2
#define LSBCRC7	0xD3
#define CONFIG5	0xD8

/* Tx Status Register */
#define	TSD_OWN		0x00002000
#define	TSD_TUN		0x00004000
#define	TSD_TOK		0x00008000
#define	TSD_ERTXTH	0x00010000
#define	TSD_NCC		0x01000000
#define	TSD_CDH		0x10000000
#define	TSD_OWC		0x20000000
#define	TSD_TABT	0x40000000
#define	TSD_CRS		0x80000000

/* Command Register */
#define	CR_RST	0x10
#define	CR_RE	0x08
#define	CR_TE	0x04
#define CR_BUFE	0x01

/* Masques pour les interruptions */
#define	INT_ROK		0x0001
#define	INT_RER		0x0002
#define	INT_TOK		0x0004
#define	INT_TER		0x0008
#define	INT_RXOVW	0x0010
#define	INT_PUN		0x0020
#define INT_LINKCHG	0x0020
#define INT_FOVW	0x0040
#define	INT_LENCHG	0x2000	
#define	INT_TIMEOUT	0x4000
#define	INT_SERR	0x8000

static void enable_interrupt(int interrupts);
static void disable_interrupt(int interrupts);
static void clear_interrupt(int interrupts);

/* Définitions liées à l'utilisation de la carte */
static pci_function_p pci_function = NULL;
static uint16_t io_base = 0x0000;

static char mac_addr[6];

#define READ_BYTE(offset) inb(io_base+offset)
#define READ_WORD(offset) inw(io_base+offset)
#define READ_DWORD(offset) inl(io_base+offset)

#define WRITE_BYTE(offset, value) outb(value, io_base+offset)
#define WRITE_WORD(offset, value) outw(value, io_base+offset)
#define WRITE_DWORD(offset, value) outl(value, io_base+offset)


/*=============================================*
 * 
 * 			TX
 * 
 *=============================================*/

#define TX_DESC_NUM 4
#define NEXT_TX_DESC(current) (current == TX_DESC_NUM-1)?0:(current+1)

typedef struct {
	char buffer[16 + 1500] __attribute__((aligned(4)));;
	uint16_t packet_length;
}transmit_descriptor_t;

static transmit_descriptor_t tx_desc[TX_DESC_NUM];

static uint8_t current_desc = 0;
static uint8_t first_desc = 0;
static uint8_t free_desc = TX_DESC_NUM;

#define TSD_STATUS_BOTH	3
#define TSD_STATUS_OWN	2
#define TSD_STATUS_TOK	1
#define TSD_STATUS_0	0

static uint8_t tsd_status(uint8_t desc)
{
	uint8_t ret = -1;
	uint32_t offset = desc << 2;
	uint32_t temp_tsd = READ_DWORD(TSD_BASE + offset);
	
	switch(temp_tsd & (TSD_OWN | TSD_TOK))
	{
		case (TSD_OWN | TSD_TOK):
			ret = TSD_STATUS_BOTH;
			break;
		case TSD_OWN:
			ret = TSD_STATUS_OWN;
			break;
		case TSD_TOK:
			ret = TSD_STATUS_TOK;
			break;
		default:
			ret = TSD_STATUS_0;
			break;
	}
	return ret;
}

static void issue_descriptor(uint8_t desc)
{
	uint32_t offset = desc<<2;
	uint32_t address = (uint32_t) tx_desc[desc].buffer;
	uint32_t packet_length = tx_desc[desc].packet_length;
	
	WRITE_DWORD(TSAD_BASE + offset, address);
	WRITE_DWORD(TSD_BASE + offset, packet_length);
}	

int send_packet(char* packet, uint32_t length)
{
	int ret = -1;
	
	/* Si un descripteur est libre, c'est bon */
	if(free_desc>0)
	{
		tx_desc[current_desc].packet_length = length;
		memcpy(tx_desc[current_desc].buffer, packet, length);
		issue_descriptor(current_desc);
	
		current_desc = NEXT_TX_DESC(current_desc);
		free_desc--;
		
		ret = 0;
	}
	else
		kerr("No free descriptor.");
	return ret;
}

void tx_interrupt_handler(uint16_t isr)
{
	klog("Tx Interrupt(0x%x)", isr);
	if(isr & INT_TOK)
	{
		while(tsd_status(first_desc) == TSD_STATUS_BOTH && free_desc < TX_DESC_NUM)
		{
			first_desc = NEXT_TX_DESC(first_desc);
			free_desc++;
		} 
	}
	else
			kerr("Packet transmission error.");
}

static void reset_transmitter()
{
	/* Configure le Transmit Configuration Register */
	uint32_t tcr = 0;
	tcr |= (3 << 24) 	/* Interframe Gap Time, 9.6us pour le 10Mbps et 960ns pour le 100Mbps d'après IEEE 802.3 */
	      |(6 << 8);	/* Max DMA Burst size: 1024 bytes */
	WRITE_DWORD(TCR, tcr);
	
	enable_interrupt(INT_TOK | INT_TER);
}

/*============================================*
 * 
 * 			RX
 * 
 *============================================*/
 
 typedef struct
 {
	 uint16_t rok	:1;
	 uint16_t fae	:1;
	 uint16_t crc	:1;
	 uint16_t longp	:1;
	 uint16_t runt	:1;
	 uint16_t ise	:1;
	 uint16_t 		:7; /* Reserved */
	 uint16_t bar	:1;
	 uint16_t pam	:1;
	 uint16_t mar	: 1;
	 uint16_t packet_length;
 }rx_packet_header_t;
 
 #define RX_BUF_LEN 8192
/* Buffer de réception:
   8k de buffer,
 + 16 bytes de header
 + 1,5k pour le WRAP
*/
static char	 rx_buffer[RX_BUF_LEN + 16 + 1536] __attribute__((aligned(4)));
static uint16_t rx_read_offset = 0;

static int  rx_good_packets = 0;
static int 	rx_bad_packets	= 0;

static int packet_ok(rx_packet_header_t* pheader)
{
	int ret;
	/* Un paquet n'est pas bon si... */
	int bad_packet = 	pheader->fae ||	/* Mauvais alignement de la trame */
						pheader->crc ||	/* Mauvais CRC */
						pheader->longp ||	/* Trop long pour la FIFO */
						pheader->runt;	/* Trop court (<64 bytes) */
	if(!bad_packet && pheader->rok)
	{
		rx_good_packets++;
		ret = 1;
	}
	else
	{
		rx_bad_packets++;
		ret = 0;
	}
	return ret;
}

void rx_interrupt_handler(uint16_t isr)
{
	rx_packet_header_t* pheader;
	uint16_t packet_length;
	char* read_ptr;
	char* packet_ptr;
	
	uint8_t temp_cmd;
	
	/* Temporaire, juste pour afficher le paquet */
	char* aux_ptr;
	int compteur;
	
	/* Tant que le buffer n'est pas vide */
	temp_cmd = READ_BYTE(CR);
	while(!(temp_cmd & CR_BUFE))
	{
		read_ptr = rx_buffer + rx_read_offset;
		packet_ptr = read_ptr + 4; /* 4 est la taille du header */
		
		pheader = (rx_packet_header_t*)read_ptr;
		packet_length = pheader->packet_length;
		
		if(packet_ok(pheader))
		{
			kdebug("Packet received (size=%d):", packet_length);
			/* 
			 * TODO: Récupérer le paquet et le mettre quelque part
			 * En attendant, pourquoi ne pas l'afficher :D
			 */
			 aux_ptr = packet_ptr;
			 compteur = 1;
			 kprintf("\t");
			 while(aux_ptr < packet_ptr + packet_length - 4 )
			 {
				kprintf("%x",((*aux_ptr)>>4) & 0xF);
				kprintf("%x",(*aux_ptr) & 0xF);
				if(compteur % 2 == 0)
					kprintf(" ");
				if(compteur % 16 == 0)
					kprintf("\n\t");
				compteur++;
				aux_ptr++;
			 }
			 kprintf("\n");
			 
			 
			/* On avance le pointeur de lecture, en gardant l'alignement sur les 4 bytes */
			rx_read_offset += (packet_length + 4 + 3) & ~3;
			rx_read_offset = rx_read_offset % RX_BUF_LEN;
			WRITE_WORD(CAPR, rx_read_offset - 0x10);
		}
		else
		{
			kerr("Bad packet(header=0x%x, isr=0x%x).\n", *((uint32_t*)pheader), isr);
			break;
		}
		
		temp_cmd = READ_BYTE(CR);
	}

}

static void reset_receiver()
{
	uint32_t rcr = 0;
	paddr_t buffer_address = (paddr_t) rx_buffer;
	
	rx_read_offset = 0;
	rx_good_packets = 0;
	rx_bad_packets = 0;
	
	rcr |=    (1<<1)	/* Accept Physical Match Packets */
			| (1<<2)	/* Accept Multicast Packets */
			| (1<<3)	/* Accept Broadcast Packets */
			| (1<<4)	/* Accept Runt Packets (ie paquets < 64bits) */
			| (1<<5)	/* Accept Error Packets (CRC, alignement & collision) */
			| (1<<7)	/* WRAP enabled (Le buffer prend 1.5k en plus, mais les paquets sont toujours continues dans le buffer) */
			| (6<<8)	/* 1024 byte Max DMA Burst */
			| (0<<11)	/* Buffer reception:
						 * 00: 8k + 16 bytes
						 * 01: 16k + 16 bytes
						 * 10: 32k + 16 bytes
						 * 11: 64k + 16 bytes (WRAP interdit) 
						 */
			| (6<<14)	/* Rx FIFO Threshold:
						 * 000: 16 bytes
						 * 001: 32 bytes
						 * 010: 64 bytes
						 * 011: 128 bytes
						 * 100: 256 bytes
						 * 101: 512 bytes
						 * 110: 1024 bytes
						 * 111: no threshold (transfert de données uniquement quand un paquet entier arrive
						 */
			| (1<<24);
			
	WRITE_DWORD(RCR, rcr);
	
	WRITE_DWORD(RBSTART, buffer_address);
	//WRITE_WORD(CAPR, 0x00);
	
	enable_interrupt(INT_ROK | INT_RER | INT_RXOVW | INT_FOVW);
}

/*============================================*
 * 
 * 			Interrupt Handling
 * 
 *=============================================*/
 
static void clear_interrupt(int interrupts)
{
	WRITE_WORD(ISR, interrupts);
} 

static void enable_interrupt(int interrupts)
{
	uint16_t current_mask = READ_WORD(IMR);
	current_mask |= interrupts;
	WRITE_WORD(IMR, current_mask);
}


static void disable_interrupt(int interrupts)
{
	uint16_t current_mask = READ_WORD(IMR);
	current_mask &= (~interrupts);
	WRITE_WORD(IMR, current_mask);
}


void rtl8139_isr(int id __attribute__ ((unused)))
{
	uint16_t temp_isr = READ_WORD(ISR);
	
	asm volatile( "cli" );
	
	kdebug("interrupt: isr=0x%x", temp_isr);

	if(temp_isr & (INT_TOK | INT_TER))
	{
		tx_interrupt_handler(temp_isr);
		clear_interrupt(INT_TOK | INT_TER);
	}
	if(temp_isr & (INT_ROK | INT_RER | INT_RXOVW | INT_FOVW))
	{
		rx_interrupt_handler(temp_isr);
		clear_interrupt(INT_ROK | INT_RER | INT_RXOVW | INT_FOVW);
	}

	asm volatile( "sti" );
}

/*=========================================*
 * 
 * 			Global Initialization
 * 
 *==========================================*/

void rtl8139_debug_info()
{
	/*
	printf("----RLT8139 DEBUG INFO----\n");
	printf("IMR:\t0x%x\n", READ_WORD(IMR));
	printf("ISR:\t0x%x\n", READ_WORD(ISR));
	printf("CR:\t0x%x\n", READ_WORD(CR));
	printf("ERBCR:\t0x%x\n", READ_WORD(ERBCR));
	printf("ERSR:\t0x%x\n", READ_BYTE(ERSR));
	printf("RBSTART:\t0x%x\n",READ_DWORD(RBSTART));
	printf("CAPR:\t0x%x\n",READ_DWORD(CAPR));
	printf("CBR:\t0x%x\n",READ_WORD(CBR));
	printf("MPC:\t0x%x\n",READ_WORD(MPC));
	printf("MSR:\t0x%x\n",READ_BYTE(MSR));
	printf("BMCR:\t0x%x\n",READ_BYTE(BMCR));
	printf("BMSR:\t0x%x\n",READ_BYTE(BMSR));
	printf("rx_read_offset:\t0x%x\n",rx_read_offset);
	int i = 0;
	uint32_t* pheader = (uint32_t *) &(rx_buffer[READ_DWORD(CBR)]);
	printf("0x%x", *pheader);
	*/
	/*for(i=0; i<4; i++)
	{
		printf("%x.",(rx_buffer[READ_WORD(CBR)])& 0x000000FF);
	}*/
	/*
	printf("\n");
	*/
	
}

static void reset_chip()
{
	/*  Met LWAKE à l'état actif haut (sorte de "power on") */
	WRITE_BYTE(CONFIG1, 0);
	
	/* 
	 * Envoi de la commande de software reset 
	 */
	WRITE_BYTE(CR, CR_RST);
	while( (READ_BYTE(CR) & CR_RST) >> 4 ); /* On attend que le reset ai bien eu lieu (~2ms en réel) */
	
	/* Totalement con et inutile */
	disable_interrupt(INT_ROK | INT_TOK);
	
	reset_transmitter();
	reset_receiver();
	//enable_interrupt(INT_PUN | INT_SERR);
	
	WRITE_BYTE(CR, CR_RE | CR_TE);
}

int rtl8139_driver_init()
{
	int ret = -1;
	uint32_t bar = 0;
	
	/* Chercher si le matériel est présent sur le bus PCI*/
	pci_function = NULL;
	pci_function = pci_find_device(RTL8139_VENDOR_ID, RTL8139_DEVICE_ID);
	if(pci_function != NULL)
	{
		klog("RTL8139 found at (%d:%d:%d)...", pci_function->bus, pci_function->slot, pci_function->function);
		/* L'adresse I/O est sensée se trouver dans le BAR0 */
		bar = pci_read_value(pci_function, PCI_BAR0);
		if( (bar & 1) ) /* C'est bien une adresse I/O, on peut initialiser */
		{
			io_base = bar & 0xfff0;
			
			kdebug("Reseting chip... ");
			reset_chip();
			mac_addr[0] = READ_BYTE(IDR0);
			mac_addr[1] = READ_BYTE(IDR1);
			mac_addr[2] = READ_BYTE(IDR2);
			mac_addr[3] = READ_BYTE(IDR3);
			mac_addr[4] = READ_BYTE(IDR4);
			mac_addr[5] = READ_BYTE(IDR5);
			
			/* ça c'est juste pour tester le port I/O... et puis c'est class */
			kdebug("MAC Address: %x:%x:%x:%x:%x:%x", READ_BYTE(IDR0)
														, READ_BYTE(IDR1)
														, READ_BYTE(IDR2)
														, READ_BYTE(IDR3)
														, READ_BYTE(IDR4)
														, READ_BYTE(IDR5));
	
			ret = pci_read_value(pci_function, PCI_INTERRUPT_LINE);
			
		}
		else
			kerr("No I/O base address found.");
	}
	else
	{
		kerr("RTL8139 introuvable.");
	}
	
	if(ret > 0)
		klog("RTL8139 driver initialized.");
	else
		klog("RTL8139 driver not initialized.");
	
	
	return ret;
}
