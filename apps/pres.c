#include <stdio.h>
#include <gui.h>
#include <process.h>

struct widget_t* schema_vmm(struct window_t *win) {
return addTxt(win, "                    ^                                   ^\n"
"                    |                                   |\n"
"  Fin de la zone    |                                   |\n"
"  gérée par     --->|-----------------------------------| -\n"
"  vmm.c (vmm_top)   |                                   | | Bloc de\n"
"                    |-----------------------------------| | 2 pages\n"
"                    |-----------------------------------| -\n"
"                    | Zone pages virtuelles non mappées | | \n"
"                    |      à des pages physiques.       | | Bloc de\n"
"                    |           (zone libre)            | | 4 pages\n"
"                    |-----------------------------------| |\n"
"                    |-----------------------------------| -\n"
"                    |Zone pages virtuelles mappées à des| | Bloc de    \n"
"                    | pages physiques. (zone utilisée)  | | 3 pages \n"
"                    |-----------------------------------| | (= 12 kio)\n"
"    Début de la     |Structure décrivant le bloc (16 o) | |\n"
"    zone gérée  --->|-----------------------------------| -\n"
"    par vmm.c       |       Noyau - Identity Mapped     |\n"
"                    |___________________________________|\n");
}

struct widget_t* schema_pagination(struct window_t *win) {
return addTxt(win, "Adresse : \033[32m########## \033[31m########## \033[34m############\033[0m\n"
"          <--------> <--------> <---------->\n"
"            |             |________    |_______________\n"
"            |            Indice de |        Offset dans|\n"
"            |             la page  |         la page   |\n"
"      Indice|    ________          |    ________       |    ________ \n"
"      de la |   |        |         |   |        |      |   |        |\n"
"      table |   |________|         |   |________|      |   |        |\n"
"            |   | @ phys |         |   | @ phys |      |   |        |\n"
"            |-->|  table |--|      |-->|  page  |---|  |-->|________|\n"
"                |________|  |          |________|   |      |        |\n"
"                |        |  |          |        |   |      |        |\n"
"                |........|  |          |........|   |      |........|\n"
"                |        |  |          |        |   |      |        |\n"
"          CR3-->|________|  |--------->|________|   |----->|________|\n"
"               Repertoire de         Table de pages       Page (4 kio)\n"
"              tables de pages        (1024 entrees)\n");
}

int main_pres(int argc __attribute__ ((unused)), char* argv[] __attribute__ ((unused)))
{
	struct window_t* win;
	struct widget_t* bck;
	struct widget_t* nxt;
	struct widget_t* txt;
		
	win = createWindow(0, 7);

	bck = addButton(win,"Prec");
	nxt = addButton(win,"Suiv");
	//txt = addTxt(win, "Bonjour, voici blabla nininninininininininininininininfefjkrjlknvfnflbngfklbjgkbjklgbjgkbjgklb\nEt nana, et sisi ...\nEt vlan et bim !!!!");
	txt = schema_pagination(win);
	//txt = schema_vmm(win);
	setWidgetProperties(bck,  1 , 22, 3, 6, 5, 0);
	setWidgetProperties(nxt, 73, 22, 3, 6, 5, 0);
	setWidgetProperties(txt, 1 , 1, 20, 78, 7, 0);

	runWindow(win);
	
	return 0;
}

int launch_pres()
{
	exec(main_pres, "presentation");
	return 0;
}

