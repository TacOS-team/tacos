/**
 * @file widget.c
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

#include <fcntl.h>
#include <unistd.h>
#include <gui.h>
#include <widget.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>

size_t ansi_strlen(const char* s)
{
	int len=0;
	int i=0;
	while(s[i])	{
		if(s[i] == '\033') {
			len--;
			i++;
			while(isdigit(s[i]) || s[i] == ';' || s[i] == '[')
				i++;
		}
		i++;
		len++;
	}
	return len;
}

char *ansi_strncpy(char * s1, const char * s2, size_t n)
{
	size_t i,j;
	char escaped = 0;
	for(i=0 ; s2[i] != '\0' && n>0 ; i++)
	{
		s1[i] = s2[i];
		if(escaped) {
			escaped = (isdigit(s2[i]) || s2[i] == ';' || s2[i] == '[');
		}
		else {
			if(s2[i] == '\033')
				escaped = 1;
			else
				n--;
		}
	}
	for(j=0 ; n>0 ; j++)
	{
		n--;
		s1[i+j] = '\0';
	}
	
	return s1;
}

static void displayWidget(struct widget_t* wdg)
{
	int x, y;
	char ligne[512]; // buffer tres long car il peut stocker des caracteres ANSI étendus
	ligne[0] = '\0';
	int i=0;
	switch(wdg->type)
	{
		case BUTTON :
			for(x=wdg->x ; x<(wdg->x+wdg->w) ; x++)
			{
				ligne[i] = ' ';
				i++;
				ligne[i] = '\0';
			}
			for(y=wdg->y ; y<(wdg->y+wdg->h) ; y++)
				printf("\033[%d;%dH\033[3%dm\033[10%dm%s",y+1,wdg->x+1,wdg->fg,wdg->bg,ligne);
			x = wdg->x + (wdg->w - ansi_strlen(wdg->adv))/2;
			y = wdg->y + (wdg->h)/2;
			printf("\033[%d;%dH\033[3%dm\033[10%dm%s",y+1,x+1,wdg->fg,wdg->bg,wdg->adv);
			break;
		case TXT :
			for(x=wdg->x ; x<(wdg->x+wdg->w) ; x++)
			{
				ligne[i] = ' ';
				i++;
				ligne[i] = '\0';
			}
			for(y=wdg->y ; y<(wdg->y+wdg->h) ; y++)
				printf("\033[%d;%dH\033[3%dm\033[10%dm%s",y+1,wdg->x+1,wdg->fg,wdg->bg,ligne);
			x = wdg->x + 2;
			y = wdg->y + 2;
			int indice = 0;
			char fini = 0;
			while(!fini)
			{
				// On recupere une ligne de la largeur du widget
				if((unsigned char)ansi_strlen(&(wdg->adv[indice]))<(wdg->w-2))
				{
					ansi_strncpy(ligne, &(wdg->adv[indice]), wdg->w-2);

					if(strchr(ligne, '\n') == NULL)
						fini = 1;
				}
				else
				{
					ansi_strncpy(ligne, &(wdg->adv[indice]), wdg->w-2);
	//				ligne[wdg->w-2] = '\0'; //Mettre le caractere en vrai fin de string, est-ce necessaire ?
				} 
				// On coupe la ligne si elle contient un retour à la ligne
				char* ret = strchr(ligne, '\n');
				if(ret != NULL)
				{
					*ret = '\0';
					indice++;
				}
				// On print la ligne
				indice+=strlen(ligne);
				printf("\033[%d;%dH\033[3%dm\033[10%dm%s",y,x,wdg->fg,wdg->bg,ligne);
				y++;
			}
			break;
		default :
			break;
	}
}

struct window_t* createWindow(unsigned char bg, unsigned char cursor)
{
	struct window_t* ret;
	int i;
	ret = malloc(sizeof(struct window_t));
	ret->bg = bg;
	ret->cursor = cursor;
	ret->nb_widgets = 0;
	for(i=0 ; i<MAX_WIDGETS ; i++) ret->widgets[i] = NULL;
	return ret;
}

void displayWindow(struct window_t* win)
{
	int i;

	printf("\033[10%dm\033[2J",win->bg);

	for(i=0 ; i<win->nb_widgets ; i++)
	{
		if(win->widgets[i]->visible)
			displayWidget(win->widgets[i]);
	}

	fflush(stdout);
	
   // XXX: Cette fonction n'est pas exposée en userspace.
   // disable_cursor(1);
}

void runWindow(struct window_t* win)
{
	int x;
	int y;
	int btn_frz = 0;
	int i;
	int old_x = 0;
	int old_y = 0;

	int fd = open("/dev/mouse", O_RDONLY);

	struct mousestate {
		unsigned int x : 13;
		unsigned int y : 13;
		unsigned int b1 : 1;
		unsigned int b2 : 1;
		unsigned int b3 : 1;
		unsigned int b4 : 1;
		unsigned int b5 : 1;
		unsigned int b6 : 1;
	} mousedata;

	displayWindow(win);
	while(1)
	{
		read(fd, &mousedata, sizeof(mousedata));
		x = (mousedata.x * 79) / 319;
		y = 24 - (mousedata.y * 24) /199;
		if ((x != old_x || y != old_y) && !(x == 79 && y == 24)) {
			// Dernière condition car si on est tout en bas à gauche, une nouvelle ligne apparaît :/
			displayWindow(win);
			printf("\033[%d;%dH\033[10%dm ", y + 1, x + 1, 6);
			fflush(stdout);
			old_x = x;
			old_y = y;
		}

		if(mousedata.b1)
		{
			if(!btn_frz)
			{
				for(i=0 ; i<win->nb_widgets ; i++)
				{
					if(win->widgets[i]->onClick != NULL &&
							x >= win->widgets[i]->x &&
							x < win->widgets[i]->x + win->widgets[i]->w &&
							y >= win->widgets[i]->y &&
							y < win->widgets[i]->y + win->widgets[i]->h
					  ) {
						win->widgets[i]->onClick(win->widgets[i],x,y);
						displayWindow(win);
					}
				}
			}
			btn_frz = 1;
		}
		else
		{
			btn_frz = 0;
		}
		usleep(10000);
	}
	
	close(fd);
}

void freeWindow(struct window_t* win)
{
	// TODO : free correct (actuellement on ne libere pas les widgets)
	free(win);
};

struct widget_t* addButton(struct window_t* win, const char* title)
{
	struct widget_t* ret;

	if(win->nb_widgets >= MAX_WIDGETS)
		return NULL;

	// On initialise le widget
	ret  = malloc(sizeof(struct widget_t));
	ret->adv = strdup(title);
	ret->type = BUTTON;
	ret->x = 0;
	ret->y = 0;
	ret->w = 0;
	ret->h = 0;
	ret->fg = 0;
	ret->bg = win->bg;
	ret->father = win;
	ret->onClick = NULL;
	ret->visible = 1;

	// On ajoute le widget a la window
	win->widgets[win->nb_widgets] = ret;
	win->nb_widgets++;

	return ret;
}

struct widget_t* addTxt(struct window_t* win, const char* txt)
{
	struct widget_t* ret;
	ret = addButton(win, txt);

	if(ret != NULL)
		ret->type = TXT;

	return ret;
}

void setWidgetProperties(struct widget_t* wdg, 
									unsigned char x, unsigned char y,
									unsigned char h, unsigned char w,
									unsigned char bg, unsigned char fg)
{
	wdg->x = x;
	wdg->y = y;
	wdg->w = w;
	wdg->h = h;
	wdg->fg = fg;
	wdg->bg = bg;
}

void setOnClick(struct widget_t* wdg, void (*onClick)(struct widget_t*, int, int))
{
	wdg->onClick = onClick;
}

void setVisible(struct widget_t* wdg, char visible)
{
	wdg->visible = visible;
}

void setText(struct widget_t* wdg, const char* text) {
  free(wdg->adv);
  wdg->adv = strdup(text);
}

