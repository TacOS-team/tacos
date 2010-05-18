#include <stdio.h>
#include <gui.h>
#include <process.h>

int main_pres(int argc __attribute__ ((unused)), char* argv[] __attribute__ ((unused)))
{
	struct window_t* win;
	struct widget_t* bck;
	struct widget_t* nxt;
	struct widget_t* txt;
		
	win = createWindow(0, 7);

	bck = addButton(win,"Prec");
	nxt = addButton(win,"Suiv");
	txt = addTxt(win, "Bonjour, voici blabla nininninininininininininininininfefjkrjlknvfnflbngfklbjgkbjklgbjgkbjgklb\nEt nana, et sisi ...\nEt vlan et bim !!!!");
	setWidgetProperties(bck,  1 , 20, 3, 6, 5, 0);
	setWidgetProperties(nxt, 70 , 20, 3, 6, 5, 0);
	setWidgetProperties(txt, 1 , 1, 18, 78, 3, 0);

	runWindow(win);
	
	return 0;
}

int launch_pres()
{
	create_process("tache_presentation", (paddr_t)main_pres, 2, NULL, 512, 3);
	return 0;
}

