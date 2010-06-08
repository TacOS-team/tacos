#include <gui.h>
#include <widget.h>
#include <kmalloc.h>
#include <string.h>
#include <stdio.h>
#include <mouse.h>
#include <video.h>
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
				printf("\033[%d;%dH\033[3%dm\033[4%dm%s",y+1,wdg->x+1,wdg->fg,wdg->bg,ligne);
			x = wdg->x + (wdg->w - ansi_strlen(wdg->adv))/2;
			y = wdg->y + (wdg->h)/2;
			printf("\033[%d;%dH\033[3%dm\033[4%dm%s",y+1,x+1,wdg->fg,wdg->bg,wdg->adv);
			break;
		case TXT :
			for(x=wdg->x ; x<(wdg->x+wdg->w) ; x++)
			{
				ligne[i] = ' ';
				i++;
				ligne[i] = '\0';
			}
			for(y=wdg->y ; y<(wdg->y+wdg->h) ; y++)
				printf("\033[%d;%dH\033[3%dm\033[4%dm%s",y+1,wdg->x+1,wdg->fg,wdg->bg,ligne);
			x = wdg->x + 2;
			y = wdg->y + 2;
			int indice = 0;
			char fini = 0;
			while(!fini)
			{
				// On recupere une ligne de la largeur du widget
				if(ansi_strlen(&(wdg->adv[indice]))<(wdg->w-2))
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
				printf("\033[%d;%dH\033[3%dm\033[4%dm%s",y,x,wdg->fg,wdg->bg,ligne);
				y++;
			}
			break;
		default :
			break;
	}
}

struct window_t* createWindow(uint8_t bg, uint8_t cursor)
{
	struct window_t* ret;
	int i;
	ret = kmalloc(sizeof(struct window_t));
	ret->bg = bg;
	ret->cursor = cursor;
	ret->nb_widgets = 0;
	for(i=0 ; i<MAX_WIDGETS ; i++) ret->widgets[i] = NULL;
	return ret;
}

void displayWindow(struct window_t* win)
{
	int i;

	printf("\033[4%dm\033[2J",win->bg);

	for(i=0 ; i<win->nb_widgets ; i++)
	{
		if(win->widgets[i]->visible)
			displayWidget(win->widgets[i]);
	}

	fflush(stdout);
	disable_cursor(1);
}

void runWindow(struct window_t* win)
{
	int frst_time = 1;
	int x;
	int y;
	int btn_frz = 0;
	int divider=0;
	int i;
	int old_x = 0;
	int old_y = 0;
	int colfg = 2;
	int colbg = 5;

	displayWindow(win);
	while(1)
	{
		if(divider%100000 == 0 && get_current_process() == get_active_process())
		{
			getMouseCoord(&x,&y);
			x = x*80/640;
			y = 25 - (y*25/480);

			if(!frst_time)
				set_attribute_position(colbg, colfg, old_x, old_y);
			else
				frst_time = 0;
			colfg = get_fg_position(x,y);
			colbg = get_bg_position(x,y);
			set_attribute_position(win->cursor, BLACK, x, y);
			old_x = x;
			old_y = y;
	
			if(getMouseBtn(0))
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
		}
	divider++;
	}
}

int freeWindow(struct window_t* win)
{
	// TODO : free correct (actuellement on ne libere pas les widgets)
	return kfree(win);
};

struct widget_t* addButton(struct window_t* win, const char* title)
{
	struct widget_t* ret;

	if(win->nb_widgets >= MAX_WIDGETS)
		return NULL;

	// On initialise le widget
	ret  = kmalloc(sizeof(struct widget_t));
	ret->adv = kmalloc(sizeof(char)*strlen(title)+1);
	strcpy(ret->adv, title);
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
									uint8_t x, uint8_t y,
									uint8_t h, uint8_t w,
									uint8_t bg, uint8_t fg)
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

