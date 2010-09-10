/**
 * @file mouse.c
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

#include <types.h>
#include <stdio.h>
#include <ioports.h>
#include <mouse.h>
#include <interrupts.h>

static uint8_t mouse_cycle;
static int mouse_x;
static int mouse_y;
static bool mouse_btn[5];
static int packet_x_sign;
static int packet_y_sign;

#define SCREEN_HEIGHT 480
#define SCREEN_WIDTH 640

static void mouse_wait(unsigned char type)
{
  unsigned int _time_out=100000;
  if(type==0)
  {
    while(_time_out--) //Data
    {
      if((inb(0x64) & 1)==1)
      {
        return;
      }
    }
    return;
  }
  else
  {
    while(_time_out--) //Signal
    {
      if((inb(0x64) & 2)==0)
      {
        return;
      }
    }
    return;
  }
}

static void mouse_write(unsigned char a_write)
{
   //Wait to be able to send a command
   mouse_wait(1);
   //Tell the mouse we are sending a command
   outb(0xD4,0x64);
   //Wait for the final part
   mouse_wait(1);
   //Finally write
   outb(a_write, 0x60);
}

static unsigned char mouse_read()
{
   //Get response from mouse
   mouse_wait(0);
   return inb(0x60);
}

void mouseInit()
{
	// Initialisation des variables
	int i;
	mouse_x = SCREEN_WIDTH/2;
	mouse_y = SCREEN_HEIGHT/2;
	mouse_cycle = 0;
	
	for(i=0 ; i<5 ; i++)
		mouse_btn[i] = FALSE;
		
	// initialisation de la souris
	mouse_wait(1);
	outb(0xA8,0x64);
	mouse_wait(1);
	outb(0x20,0x64);
	
	unsigned char status_byte;
	mouse_wait(0);
	status_byte = (inb(0x60) | 2);
	mouse_wait(1);
	outb(0x60,0x64);
	mouse_wait(1);
	outb(status_byte,0x60);
	// on dit à la souris d'utiliser les options par defaut
	mouse_write(0xF6);
	mouse_read();
	// on active la souris
	mouse_write(0xF4);
	mouse_read();
	
	interrupt_set_routine(IRQ_PS2_MOUSE, mouseInterrupt, 0);
}

void mouseInterrupt(int id __attribute__ ((unused)))
{ 
  static bool first = TRUE;
  uint8_t packet;
  int delta;
  
  if(first) // On ignore la premiere interruption
  {
	  first = FALSE;
	  return;
  }
  
  switch(mouse_cycle)
  {
    case 0: // premier paquet
	  packet = inb(0x60);
	  // boutons de la souris
	  mouse_btn[0] = packet & (0x01 << 0);
	  mouse_btn[1] = (packet & (0x01 << 1)) >> 1;
	  mouse_btn[2] = (packet & (0x01 << 2)) >> 2;
	  packet_x_sign = packet & (0x01 << 4) ? 0xFFFFFF00 : 0 ; // bit de signe pour dx
	  packet_y_sign = packet & (0x01 << 5) ? 0xFFFFFF00 : 0 ; // bit de signe pour dy
      mouse_cycle++;
      //printf("cycle 0 : %x\n",packet);
      break;
    case 1:	// delta X
      packet = inb(0x60);
      delta = packet_x_sign | packet;
      mouse_x += delta;
      if(mouse_x > SCREEN_WIDTH)
		mouse_x = SCREEN_WIDTH;
	  if(mouse_x < 0)
		mouse_x = 0;
      mouse_cycle++;
      //printf("cycle 1 : %x\n",packet);
      break;
    case 2: // delta Y
      packet = inb(0x60);
      delta = packet_y_sign | packet;
      mouse_y += delta;
      if(mouse_y > SCREEN_HEIGHT)
		mouse_y = SCREEN_HEIGHT;
	  if(mouse_y < 0)
		mouse_y = 0;
      mouse_cycle=0;
      //printf("cycle 2 : %x\n",packet);
      break;
  }
}

void getMouseCoord(int* x, int* y)
{
	*x = mouse_x;
	*y = mouse_y;
}

int getMouseBtn(int btnId)
{
	if(btnId >= 0 && btnId < 5)
		return mouse_btn[btnId];
	return 0;
}
