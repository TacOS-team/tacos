#include <stdio.h>
#include <gui.h>
#include <process.h>

#define NB_SLIDES 24

int current = 0;
struct widget_t* title[NB_SLIDES];
struct widget_t* txt[NB_SLIDES];
struct widget_t* page;

void intro(struct window_t *win, struct widget_t** txt, struct widget_t** title) {
	*txt = addTxt(win, 
"\n"
"\n"
"\n"
"\n"
"\n"
"           \033[32m    +---------------------------------------------+  \033[30m\n"
"           \033[32m    |\033[30m R�alisation d'un syst�me d'exploitation x86 \033[32m|  \033[30m\n"
"           \033[32m    +---------------------------------------------+  \033[30m\n"
"\n"
"\n"
"\n"
"\n"
"\n"
" Maxime Ch�ramy\n"
" Nicolas Floquet\n"
" Benjamin Hautbois                                              9 juin 2010\n"
" Ludovic Rigal                                4�me Ann�e G�nie Informatique\n"
" Simon Vernhes                                                INSA Toulouse\n");
	*title = addButton(win,"Pr�sentation de projet tutor�");
}

void plan(struct window_t *win, struct widget_t** txt, struct widget_t** title) {
	*txt = addTxt(win, 
"\n"
"\033[31m      I) M�moire\n"
"\033[34m         a. Pagination\n"
"\033[34m         b. Allocation m�moire\n"
"\033[31m      II) Interruptions\n"
"\033[34m         a. IDT \n"
"\033[34m         b. Wrapper\n"
"\033[34m         c. Gestion du temps\n"
"\033[31m      III) Gestion des processus\n"
"\033[34m         a. Changement de contexte\n"
"\033[34m         b. Appels syst�me\n"
"\033[34m         c. Ordonnancement\n"
"\033[31m      IV) Pilotes\n"
"\033[34m         a. Clavier/Souris\n"
"\033[34m         b. Disquette\n"
"\033[31m      V) Entr�es/sorties\n"
"\033[34m         a. Syst�me de fichier FAT\n"
"\033[34m         b. Stdin/Stdout\n");
	*title = addButton(win,"Sommaire");
}
void plan1(struct window_t *win, struct widget_t** txt, struct widget_t** title) {
	*txt = addTxt(win, 
"\n"
"\033[31m    | I) M�moire\n"
"\033[31m    |    a. Pagination\n"
"\033[31m    |    b. Allocation m�moire\n"
"\033[30m      II) Interruptions\n"
"\033[30m         a. IDT \n"
"\033[30m         b. Wrapper\n"
"\033[30m         c. Gestion du temps\n"
"\033[30m      III) Gestion des processus\n"
"\033[30m         a. Changement de contexte\n"
"\033[30m         b. Appels syst�me\n"
"\033[30m         c. Ordonnancement\n"
"\033[30m      IV) Pilotes\n"
"\033[30m         a. Clavier/Souris\n"
"\033[30m         b. Disquette\n"
"\033[30m      V) Entr�es/sorties\n"
"\033[30m         a. Syst�me de fichier FAT\n"
"\033[30m         b. Stdin/Stdout\n");
	*title = addButton(win,"I) M�moire");
}
void plan2(struct window_t *win, struct widget_t** txt, struct widget_t** title) {
	*txt = addTxt(win, 
"\n"
"\033[30m      I) M�moire\n"
"\033[30m         a. Pagination\n"
"\033[30m         b. Allocation m�moire\n"
"\033[31m    | II) Interruptions\n"
"\033[31m    |    a. IDT \n"
"\033[31m    |    b. Wrapper\n"
"\033[31m    |    c. Gestion du temps\n"
"\033[30m      III) Gestion des processus\n"
"\033[30m         a. Changement de contexte\n"
"\033[30m         b. Appels syst�me\n"
"\033[30m         c. Ordonnancement\n"
"\033[30m      IV) Pilotes\n"
"\033[30m         a. Clavier/Souris\n"
"\033[30m         b. Disquette\n"
"\033[30m      V) Entr�es/sorties\n"
"\033[30m         a. Syst�me de fichier FAT\n"
"\033[30m         b. Stdin/Stdout\n");
	*title = addButton(win,"II) Interruptions");
}
void plan3(struct window_t *win, struct widget_t** txt, struct widget_t** title) {
	*txt = addTxt(win, 
"\n"
"\033[30m      I) M�moire\n"
"\033[30m         a. Pagination\n"
"\033[30m         b. Allocation m�moire\n"
"\033[30m      II) Interruptions\n"
"\033[30m         a. IDT \n"
"\033[30m         b. Wrapper\n"
"\033[30m         c. Gestion du temps\n"
"\033[31m    | III) Gestion des processus\n"
"\033[31m    |    a. Changement de contexte\n"
"\033[31m    |    b. Appels syst�me\n"
"\033[31m    |    c. Ordonnancement\n"
"\033[30m      IV) Pilotes\n"
"\033[30m         a. Clavier/Souris\n"
"\033[30m         b. Disquette\n"
"\033[30m      V) Entr�es/sorties\n"
"\033[30m         a. Syst�me de fichier FAT\n"
"\033[30m         b. Stdin/Stdout\n");
	*title = addButton(win,"III) Gestion des processus");
}
void plan4(struct window_t *win, struct widget_t** txt, struct widget_t** title) {
	*txt = addTxt(win, 
"\n"
"\033[30m      I) M�moire\n"
"\033[30m         a. Pagination\n"
"\033[30m         b. Allocation m�moire\n"
"\033[30m      II) Interruptions\n"
"\033[30m         a. IDT \n"
"\033[30m         b. Wrapper\n"
"\033[30m         c. Gestion du temps\n"
"\033[30m      III) Gestion des processus\n"
"\033[30m         a. Changement de contexte\n"
"\033[30m         b. Appels syst�me\n"
"\033[30m         c. Ordonnancement\n"
"\033[31m    | IV) Pilotes\n"
"\033[31m    |    a. Clavier/Souris\n"
"\033[31m    |    b. Disquette\n"
"\033[30m      V) Entr�es/sorties\n"
"\033[30m         a. Syst�me de fichier FAT\n"
"\033[30m         b. Stdin/Stdout\n");
	*title = addButton(win,"IV) Pilotes");
}
void plan5(struct window_t *win, struct widget_t** txt, struct widget_t** title) {
	*txt = addTxt(win, 
"\n"
"\033[30m      I) M�moire\n"
"\033[30m         a. Pagination\n"
"\033[30m         b. Allocation m�moire\n"
"\033[30m      II) Interruptions\n"
"\033[30m         a. IDT \n"
"\033[30m         b. Wrapper\n"
"\033[30m         c. Gestion du temps\n"
"\033[30m      III) Gestion des processus\n"
"\033[30m         a. Changement de contexte\n"
"\033[30m         b. Appels syst�me\n"
"\033[30m         c. Ordonnancement\n"
"\033[30m      IV) Pilotes\n"
"\033[30m         a. Clavier/Souris\n"
"\033[30m         b. Disquette\n"
"\033[31m    | V) Entr�es/sorties\n"
"\033[31m    |    a. Syst�me de fichier FAT\n"
"\033[31m    |    b. Stdin/Stdout\n");
	*title = addButton(win,"V) Entr�es/sorties");
}

/*
 * Slides gestion m�moire :
 */

void intro_pagination(struct window_t *win, struct widget_t** txt, struct widget_t** title) {
	*txt = addTxt(win, "Pourquoi la pagination ?\n"
" \n"
"     - S�curit� (cloisement de la m�moire par processus, droits)\n"
" \n"
"     - M�moire virtuelle : Possibilit� d'utiliser plusieurs supports (swap)\n"
"                            Zones m�moires contig�es virtuellement\n"
" \n"
"     - Concept important et tr�s r�pandu : Int�r�t p�dagogique\n"
" \n"
"R�le de la MMU (Memory Management Unit) :\n"
" \n"
"     - Traduction des adresses virtuelles en adresses physiques\n"
" \n"
"     - Lev�e d'une exception lorsqu'une adresse n'est pas mapp�e\n");
	*title = addButton(win, "Pagination");
}

void schema_pagination(struct window_t *win, struct widget_t** txt, struct widget_t** title) {
	*txt = addTxt(win, "Adresse : \033[32m########## \033[31m########## \033[34m############\033[0m\n"
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
	*title = addButton(win,"Pagination");
}

void pagination1(struct window_t *win, struct widget_t** txt, struct widget_t** title) {
	*txt = addTxt(win, "1�re �tape : D�coupage de la m�moire en cadres de page\n"
" \n"
"   Objectif : Fournir des zones m�moires o� l'on peut mapper des pages.\n"
" \n"
" \n"
"   Maintien de 2 listes : Cadres utilis�s et Cadres libres.\n"
" \n"
"   Fonctions de reservation et de lib�ration de cadres.\n");
	*title = addButton(win, "Pagination");
}

void pagination2(struct window_t *win, struct widget_t** txt, struct widget_t** title) {
	*txt = addTxt(win, 
"2�me �tape : Pagination en Identity Mapping\n"
"\n"
"	Passage des adresses physiques aux adresses virtuelles d�licate\n"
"\n"
"	Id�e : Mapper les pages de fa�on � ce que :\n"
"             adresse virtuelle == adresse physique\n"
"\n"
"\n"
"	On ajoute une entr�e dans le r�pertoire de tables de page pour pointer \n"
"	vers lui-m�me. => Pouvoir modifier les tables de page\n"
"\n"
"	On peut ainsi acc�der � une table par l'adresse virtuelle :\n"
"					0xFFC00000 + 1024*index_page_table\n"
"\n"
"	Et au r�pertoire de tables de page par :\n"
"					0xFFFFF000\n");
	*title = addButton(win, "Pagination");
}


void pagination3(struct window_t *win, struct widget_t** txt, struct widget_t** title) {
	*txt = addTxt(win, 
"3�me �tape : Mise en place du VMM (Virtual Memory Manager)\n"
"\n"
"   R�le : S'occuper de g�rer des blocs de pages contig�es en \n"
"          m�moire virtuelle.\n"
"\n"
"   Maintien de 2 listes : Blocs libres et Blocs utilis�s.\n"
"\n"
"\n"
"\n"
"4�me �tape : Mise en place du kmalloc\n"
"\n"
"	Tr�s similaire au VMM mais avec une gestion � l'octet.\n");
	*title = addButton(win, "Pagination");
}


void schema_vmm(struct window_t *win, struct widget_t** txt, struct widget_t** title) {
	*txt = addTxt(win, "                    ^                                   ^\n"
"                    |                                   |\n"
"  Fin de la zone    |                                   |\n"
"  g�r�e par     --->|-----------------------------------| -\n"
"  vmm.c (vmm_top)   |\033[43m         (zone utilis�e)           \033[47m| | Bloc de\n"
"                    |\033[44m\033[34m-----------------------------------\033[47m\033[030m| | 2 pages\n"
"                    |-----------------------------------| -\n"
"                    | Zone pages virtuelles non mapp�es | | \n"
"                    |      � des pages physiques.       | | Bloc de\n"
"                    |           (zone libre)            | | 4 pages\n"
"                    |\033[44m\033[34m-----------------------------------\033[47m\033[030m| |\n"
"                    |-----------------------------------| -\n"
"                    |\033[43mZone pages virtuelles mapp�es � des\033[47m| | Bloc de    \n"
"                    |\033[43m pages physiques. (zone utilis�e)  \033[47m| | 3 pages \n"
"                    |-----------------------------------| | (= 12 kio)\n"
"    D�but de la     |\033[44m\033[37mStructure d�crivant le bloc (16 o) \033[47m\033[030m| |\n"
"    zone g�r�e  --->|-----------------------------------| -\n"
"    par vmm.c       |       Noyau - Identity Mapped     |\n"
"                    |___________________________________|\n");
	*title = addButton(win,"Pagination");
}

void pagination_prob(struct window_t *win, struct widget_t** txt, struct widget_t** title) {
	*txt = addTxt(win, 
"Syst�me tr�s complexe et avec beaucoup d'interd�pendances.\n"
"\n"
"Cons�quences lourdes sur le reste du syst�me d'exploitation.\n"
"\n"
"Bugs difficiles � d�tecter et � corriger ! \n"
"	=> Seul la MMU nous signale un probl�me.\n"
"   => Parfois un bug n'a de cons�quences que plus tard.\n");
	*title = addButton(win,"Pagination");
}

void interruptions_conf(struct window_t *win, struct widget_t** txt, struct widget_t** title) {
	*txt = addTxt(win,
"\n     Utilisations des interruptions:\n"
"          => Communication avec les p�riph�riques\n"
"          => Ordonnancement, timers...\n"
"          => Gestion des exceptions\n"
"\n\n"
"     Pour cela,\n"
"\n\n" 
"     Deux �l�ments � configurer:\n"
"          => IDT\n"
"            (Interrupt Descriptor Table)\n"
"          => PIC\n"
"            (Programmable Interrupt Controler)\n");
*title = addButton(win,"Interruptions - Configuration");
}

void schema_ordonnanceur(struct window_t *win, struct widget_t** txt, struct widget_t** title) {
	*txt = addTxt(win, 
"\n"
"      \033[30m               \033[30m                                    \033[30mEBP___\033[30m  \n"
"    pi\033[30mle kernel -----\033[30m->                                  \033[30m      \033[30m|  \n"
"  ____\033[32m __ __ __ __ __\033[34m __ __ __ __ __ __ __ __ __ __ __ __\033[31m __ __\033[30mV__\n"
"      \033[32m|  |  |  |  |  \033[34m|  |  |  |  |k |  |  |  |  |  |  |  \033[31m|k |k \033[30m|  \n"
"      \033[32m|s |e |e |c |e \033[34m|e |e |e |e |e |e |e |e |d |e |f |g \033[31m|e |e \033[30m|  \n" 
"    ..\033[32m|s |s |f |s |i \033[34m|a |c |d |b |s |b |s |d |s |s |s |s \033[31m|i |b \033[30m|..\n"
"      \033[32m|  |p |l |  |p \033[34m|x |x |x |x |p |p |i |i |  |  |  |  \033[31m|p |p \033[30m|  \n"
"  ____\033[32m|__|__|__|__|__\033[34m|__|__|__|__|__|__|__|__|__|__|__|__\033[31m|__|__\033[30m|__\n"
"      \033[30m |           | \033[30m |                                 |\033[30m |   |\033[30m   \n"
"      \033[30m |___________| \033[30m |_________________________________|\033[30m |___|\033[30m   \n"
"      \033[30m       |       \033[30m                 |                  \033[30m   |             \n"
"      \033[32mempil� par le  \033[34m    empil� apr�s l'interruption     \033[31m empil� �        \n"
"      \033[32mm�canisme d'   \033[34m                                    \033[31m l'appel de      \n"
"      \033[32minterruption   \033[34m                                    \033[31m l'ordonnanceur\033[30m\n");
	*title = addButton(win,"Ordonnanceur");
}


void slide_disquette(struct window_t *win, struct widget_t** txt, struct widget_t** title) {
	*txt = addTxt(win,
"                               \033[31mRegistres\033[30m\n"
"\n"
"\033[34mData FIFO\033[30m: envoi des commandes au controleur\n"
"\033[34mMain status register\033[30m: informations sur le status du controleur\n"
"\033[34mDigital output register\033[30m: controle du moteur, activation du DMA et de l'IRQ\n"
"\033[34mConfiguration control register\033[30m: sp�cification du d�bit de transfert\n"
"\n"
"                               \033[31mCommandes\033[30m\n"
"\n"
"\033[32mSpecify\033[30m         : donne au controleur des informations sur le lecteur\n"
"\033[32mWrite/Read data\033[30m : �crit/Lit un ou plusieurs secteurs sur la disquette\n"
"\033[32mRecalibrate\033[30m     : positionne les tetes en but�e sur le cylindre 0\n"
"\033[32mSense interrupt\033[30m : r�cup�re des informations sur une interruption\n"
"\033[32mSeek\033[30m            : d�place la tete de lecture/�criture � un cylindre donn�\n"
"\033[32mVersion\033[30m         : d�termine la version du controleur\n");
	*title = addButton(win,"Driver disquette");
}



void schema_fat(struct window_t *win, struct widget_t** txt, struct widget_t** title) {
	*txt = addTxt(win, 
"                       Partition FAT       \n"
"                \033[34m ________________________ \033[30m \n"
"                \033[34m|      Secteur de boot   |\033[30m<---*\n"
"                \033[34m|________________________|\033[30m    |\n"
"                \033[34m| File Allocation Tables |\033[30m    |\n"
"                \033[34m|________________________|\033[30m    |-- \033[34mZone r�serv�e\n"
"                \033[34m|                        |\033[30m    |\n"
"                \033[34m|    R�pertoire Racine   |\033[30m    |\n"
"                \033[34m|________________________|\033[30m<---*\n"
"                \033[32m|                        |\033[30m<---*\n"
"                \033[32m|                        |\033[30m    |\n"
"                \033[32m|       Fichiers et      |\033[30m    |\n"
"                \033[32m|    sous r�pertoires    |\033[30m    |\n"
"                \033[32m|                        |\033[30m    |-- \033[32mZone des donn�es\n" 
"                \033[32m|                        |\033[30m    |\n"
"                \033[32m|                        |\033[30m    |\n"
"                \033[32m|                        |\033[30m    |\n"
"                \033[32m|________________________|\033[30m<---*\n");
	*title = addButton(win,"Syst�me de fichiers FAT");
}

void diapo_fat(struct window_t *win, struct widget_t** txt, struct widget_t** title) {
	*txt = addTxt(win, 
"                     \033[31mInterpr�tation des valeurs de la FAT\033[30m\n"
"\n"
"\n"
"          \033[32m0x000\033[30m          Cluster vide\n"
"          \033[32m0x001\033[30m          Cluster r�serv�\n"
"          \033[32m0x002 - 0xFEF\033[30m  Pointeur vers le cluster suivant du fichier\n"
"          \033[32m0xFF0 - 0xFF6\033[30m  Valeurs r�serv�es\n"
"          \033[32m0xFF7\033[30m          Cluster d�fectueux\n"
"          \033[32m0xFF8 - 0xFFF\033[30m  Dernier cluster d'un fichier\n"
"\n"
"\n"
"                        \033[31mEntr�e r�pertoire - 32 octets\033[30m\n"
"\n"
"                          Nom | Extension | \033[34mAttributs\033[30m\n"
"                   Date/Heure cr�ation | Date dernier acc�s\n"
"                       Date/Heure derni�res modifications\n"
"                    \033[34mNo du premier cluster\033[30m | \033[34mTaille en octet\033[30m\n");

	*title = addButton(win,"Syst�me de fichiers FAT");
}


void diapo_io(struct window_t *win, struct widget_t** txt, struct widget_t** title) {
	*txt = addTxt(win, 
"                            \033[32m+-----------------+\033[30m           \n"
"                            \033[32m|     Process     |\033[30m          \n"
"  \033[34m+-----------+\033[30m             \033[32m|\033[30m  +-----------+  \033[32m|\033[30m           \033[34m+-----------+\033[30m\n"
"  \033[34m|\033[30mFlags      \033[34m|\033[30m             \033[32m|\033[30m  |     |     |  \033[32m|\033[30m           \033[34m|\033[30mFlags      \033[34m|\033[30m\n"
"  \033[34m|-----------|\033[30m             \033[32m|\033[30m  |-----------|  \033[32m|\033[30m           \033[34m|-----------|\033[30m\n"
"  \033[34m|\033[30mread_ptr   \033[34m|\033[30m         \033[31m----\033[32m|\033[31m->\033[30m|used | ofd |\033[31m--\033[32m|\033[31m---\033[30m        \033[34m|\033[30mbuffer     \033[34m|\033[30m\n"
"  \033[34m|-----------|\033[30m  \033[31mIndice\033[30m \033[31m|\033[30m   \033[32m|\033[30m  |-----------|  \033[32m|  \033[31m|\033[30m        \033[34m|-----------|\033[30m\n"
"  \033[34m|\033[30mbuffer_base\033[34m|\033[30m   \033[31mdans\033[30m  \033[31m|\033[30m   \033[32m|\033[30m  |     |     |  \033[32m|\033[30m  \033[31m|Adresse\033[30m \033[34m|           |\033[30m\n"
"  \033[34m|-----------|\033[30m \033[31mla table\033[30m\033[31m|\033[30m   \033[32m|\033[30m  |-----------|  \033[32m|\033[30m  \033[31m|\033[30m        \033[34m|\033[30m    ...    \033[34m|\033[30m\n"
"  \033[34m|\033[30mbuffer_end \033[34m|\033[30m         \033[31m|\033[30m   \033[32m|\033[30m  |     |     |  \033[32m|\033[30m  \033[31m|\033[30m        \033[34m|           |\033[30m\n"
"  \033[34m|-----------|\033[30m         \033[31m|\033[30m   \033[32m|\033[30m  |-----------|  \033[32m|\033[30m  \033[31m|\033[30m        \033[34m|-----------|\n"
"  \033[34m|\033[30mchain      \033[34m|\033[30m         \033[31m|\033[30m   \033[32m|\033[30m  |     |     |  \033[32m|\033[30m  \033[31m|\033[30m        \033[34m|\033[30mwrite()    \033[34m|\033[30m\n"
"  \033[34m|-----------|\033[30m         \033[31m|\033[30m   \033[32m|\033[30m  |-----------|  \033[32m|\033[30m  \033[31m|\033[30m        \033[34m|-----------|\033[30m\n"
"  \033[34m|\033[30mfile_no    \033[34m|\033[30m\033[31m----------\033[30m   \033[32m|\033[30m  |     |     |  \033[32m|  \033[31m|\033[30m        \033[34m|\033[30mread()     \033[34m|\033[30m\n"
"  \033[34m+-----------+\033[30m             \033[32m+\033[30m  +-----------+  \033[32m|\033[30m  \033[31m-------->\033[30m\033[34m+-----------+\033[30m\n"
"      \033[34mFILE\033[30m                  \033[32m|\033[30m      fd[]       \033[32m|\033[30m        \033[34mopen_file_descriptor\033[30m\n"
"                            \033[32m+-----------------+\033[30m");
	*title = addButton(win,"Entr�es / Sorties");
}

void souris(struct window_t *win, struct widget_t** txt, struct widget_t** title) {
	*txt = addTxt(win, 
"           \n"
"         Fonctionnement PS/2 :\n"
"           \n"
"           \n"
"            +---+   <Port[0x60]   +----+         +---------------+\n"
"            |CPU|<--------------->|PS/2|<------->|    premier    |\n"
"            +---+   Port[0x64]>   |KBC |<---+    |connecteur PS/2|\n"
"              ^                   +----+    |    +---------------+\n"
"              |                    |  |     |\n"
"           +----+     IRQ1         |  |     |\n"
"           |PIC1|<-----------------+  |     |\n"
"           +----+                     |     |    +---------------+\n"
"              ^                       |     +--->|    second     |\n"
"              |                       |          |connecteur PS/2|\n"
"           +----+     IRQ12           |          +---------------+\n"
"           |PIC2|<--------------------+\n"
"           +----+\n"
" \n");
	*title = addButton(win,"Clavier/Souris");
}

void slide_time(struct window_t *win, struct widget_t** txt, struct widget_t** title) {
	*txt = addTxt(win, 
"    \033[31m  R�cup�ration de la date :\033[30m\n"
"          communication avec la Real Time Clock (RTC)\n"
"          sur les ports 0x70 et 0x71\n"
"      \n"
"  \033[31m    Fonctionnement du PIT I8254 (Programmable Interval Timer) :\033[30m\n"
"      \n"
"                     _   _    +----------+\n"
"                   _| |_|  -> | Compteur | -> IRQ0\n"
"                              +----------+\n"
"                                   ^\n"
"                                   |\n"
"                               port 0x40\n"
"      \n"
"   \033[31m   Echelles de temps :\033[30m\n"
"         ticks -> secondes -> dates\n"
"      \n"
"  \033[31m    Planification d'�v�nements :\033[30m\n"
"         sleep, ordonnancement ...\n"
" \n");
	*title = addButton(win,"Gestion du temps");
}

void conclusion(struct window_t *win, struct widget_t** txt, struct widget_t** title) {
	*txt = addTxt(win, 
"\n"
" \n");
	*title = addButton(win,"");
}

void fin(struct window_t *win, struct widget_t** txt, struct widget_t** title) {
	*txt = addTxt(win, 
"\n"
"\n"
"\n"
"\n"
"\n"
"                          Merci pour votre attention\n"
"\n"
"\n"
"\n"
"                          Avez-vous des questions ?\033[0m\n");
	*title = addButton(win,"");
}

void goBack(struct widget_t* wdg, int x, int y)
{
  char buf[20];
  if(current <= 0)
    return;

	setVisible(title[current],0);
	setVisible(txt[current],0);

  current = (current-1);

	setVisible(title[current],1);
	setVisible(txt[current],1);

  sprintf(buf, "Page %d/%d", current+1, NB_SLIDES);
  setText(page, buf);
}

void goForward(struct widget_t* wdg, int x, int y)
{
  char buf[20];
  if(current >= NB_SLIDES-1)
    return;

	setVisible(title[current],0);
	setVisible(txt[current],0);

	current = (current+1);

	setVisible(title[current],1);
	setVisible(txt[current],1);
  
  sprintf(buf, "Page %d/%d", current+1, NB_SLIDES);
  setText(page, buf);
}

int main_pres(int argc __attribute__ ((unused)), char* argv[] __attribute__ ((unused)))
{
	struct window_t* win;
	struct widget_t* bck;
	struct widget_t* nxt;
  char buf[20];

	int i;
		
	win = createWindow(0, 7);

	i=0;
	intro(win,&txt[i],&title[i]);
	i++;
	plan(win,&txt[i],&title[i]);
	i++;
	plan1(win,&txt[i],&title[i]);
	i++;
	intro_pagination(win, &txt[i],&title[i]);
	i++;
	schema_pagination(win,&txt[i],&title[i]);
	i++;
	pagination1(win,&txt[i], &title[i]);
	i++;
	pagination2(win,&txt[i], &title[i]);
	i++;
	pagination3(win,&txt[i], &title[i]);
	i++;
	schema_vmm(win,&txt[i],&title[i]);
	i++;
	pagination_prob(win,&txt[i],&title[i]);
	i++;
	plan2(win,&txt[i],&title[i]);
	i++;
	interruptions_conf(win,&txt[i],&title[i]);
	i++;
	slide_time(win,&txt[i],&title[i]);
	i++;
	plan3(win,&txt[i],&title[i]);
	i++;
	schema_ordonnanceur(win,&txt[i],&title[i]);
	i++;
	plan4(win,&txt[i],&title[i]);
	i++;
	souris(win,&txt[i],&title[i]);
	i++;
	slide_disquette(win,&txt[i],&title[i]);
	i++;
	plan5(win,&txt[i],&title[i]);
	i++;
	schema_fat(win,&txt[i],&title[i]);
	i++;
	diapo_fat(win,&txt[i],&title[i]);
	i++;
	diapo_io(win,&txt[i],&title[i]);
	i++;
	conclusion(win,&txt[i],&title[i]);
	i++;
	fin(win,&txt[i],&title[i]);


	for(i=0 ; i<NB_SLIDES ; i++)
	{
		setVisible(txt[i],0);
		setVisible(title[i],0);
		setWidgetProperties(txt[i], 1 , 4, 20, 78, 7, 0);
		setWidgetProperties(title[i], 7, 1, 3, 66, 7, 0);
	}

	setVisible(title[current],1);
	setVisible(txt[current],1);

	bck = addButton(win,"Prec");
	setWidgetProperties(bck,  1 , 1, 3, 6, 5, 0);
	setOnClick(bck,goBack);
	nxt = addButton(win,"Suiv");
	setWidgetProperties(nxt, 73, 1, 3, 6, 5, 0);
	setOnClick(nxt,goForward);

  sprintf(buf, "Page 1/%d", NB_SLIDES);
	page = addButton(win, buf);
	setWidgetProperties(page, 60, 1, 3, 12, 3, 0);
	setOnClick(page, NULL);

	runWindow(win);
	
	return 0;
}

int launch_pres()
{
	exec(main_pres, "presentation");
	return 0;
}

