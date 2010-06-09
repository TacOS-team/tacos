#include <stdio.h>
#include <gui.h>
#include <process.h>

#define NB_SLIDES 26

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
"           \033[32m    |\033[30m Réalisation d'un système d'exploitation x86 \033[32m|  \033[30m\n"
"           \033[32m    +---------------------------------------------+  \033[30m\n"
"\n"
"\n"
"\n"
"\n"
"\n"
" Maxime Chéramy\n"
" Nicolas Floquet\n"
" Benjamin Hautbois                                              9 juin 2010\n"
" Ludovic Rigal                                4ème Année Génie Informatique\n"
" Simon Vernhes                                                INSA Toulouse\n");
	*title = addButton(win,"Présentation de projet tutoré");
}

void plan(struct window_t *win, struct widget_t** txt, struct widget_t** title) {
	*txt = addTxt(win, 
"\n"
"\033[31m      I) Mémoire\n"
"\033[34m         a. Pagination\n"
"\033[34m         b. Allocation mémoire\n"
"\033[31m      II) Interruptions\n"
"\033[34m         a. IDT \n"
"\033[34m         b. Wrapper\n"
"\033[34m         c. Gestion du temps\n"
"\033[31m      III) Gestion des processus\n"
"\033[34m         a. Changement de contexte\n"
"\033[34m         b. Appels système\n"
"\033[34m         c. Ordonnancement\n"
"\033[31m      IV) Pilotes\n"
"\033[34m         a. Clavier/Souris\n"
"\033[34m         b. Disquette\n"
"\033[31m      V) Entrées/sorties\n"
"\033[34m         a. Système de fichier FAT\n"
"\033[34m         b. Stdin/Stdout\n");
	*title = addButton(win,"Sommaire");
}
void plan1(struct window_t *win, struct widget_t** txt, struct widget_t** title) {
	*txt = addTxt(win, 
"\n"
"\033[31m    | I) Mémoire\n"
"\033[31m    |    a. Pagination\n"
"\033[31m    |    b. Allocation mémoire\n"
"\033[30m      II) Interruptions\n"
"\033[30m         a. IDT \n"
"\033[30m         b. Wrapper\n"
"\033[30m         c. Gestion du temps\n"
"\033[30m      III) Gestion des processus\n"
"\033[30m         a. Changement de contexte\n"
"\033[30m         b. Appels système\n"
"\033[30m         c. Ordonnancement\n"
"\033[30m      IV) Pilotes\n"
"\033[30m         a. Clavier/Souris\n"
"\033[30m         b. Disquette\n"
"\033[30m      V) Entrées/sorties\n"
"\033[30m         a. Système de fichier FAT\n"
"\033[30m         b. Stdin/Stdout\n");
	*title = addButton(win,"I) Mémoire");
}
void plan2(struct window_t *win, struct widget_t** txt, struct widget_t** title) {
	*txt = addTxt(win, 
"\n"
"\033[30m      I) Mémoire\n"
"\033[30m         a. Pagination\n"
"\033[30m         b. Allocation mémoire\n"
"\033[31m    | II) Interruptions\n"
"\033[31m    |    a. IDT \n"
"\033[31m    |    b. Wrapper\n"
"\033[31m    |    c. Gestion du temps\n"
"\033[30m      III) Gestion des processus\n"
"\033[30m         a. Changement de contexte\n"
"\033[30m         b. Appels système\n"
"\033[30m         c. Ordonnancement\n"
"\033[30m      IV) Pilotes\n"
"\033[30m         a. Clavier/Souris\n"
"\033[30m         b. Disquette\n"
"\033[30m      V) Entrées/sorties\n"
"\033[30m         a. Système de fichier FAT\n"
"\033[30m         b. Stdin/Stdout\n");
	*title = addButton(win,"II) Interruptions");
}
void plan3(struct window_t *win, struct widget_t** txt, struct widget_t** title) {
	*txt = addTxt(win, 
"\n"
"\033[30m      I) Mémoire\n"
"\033[30m         a. Pagination\n"
"\033[30m         b. Allocation mémoire\n"
"\033[30m      II) Interruptions\n"
"\033[30m         a. IDT \n"
"\033[30m         b. Wrapper\n"
"\033[30m         c. Gestion du temps\n"
"\033[31m    | III) Gestion des processus\n"
"\033[31m    |    a. Changement de contexte\n"
"\033[31m    |    b. Appels système\n"
"\033[31m    |    c. Ordonnancement\n"
"\033[30m      IV) Pilotes\n"
"\033[30m         a. Clavier/Souris\n"
"\033[30m         b. Disquette\n"
"\033[30m      V) Entrées/sorties\n"
"\033[30m         a. Système de fichier FAT\n"
"\033[30m         b. Stdin/Stdout\n");
	*title = addButton(win,"III) Gestion des processus");
}
void plan4(struct window_t *win, struct widget_t** txt, struct widget_t** title) {
	*txt = addTxt(win, 
"\n"
"\033[30m      I) Mémoire\n"
"\033[30m         a. Pagination\n"
"\033[30m         b. Allocation mémoire\n"
"\033[30m      II) Interruptions\n"
"\033[30m         a. IDT \n"
"\033[30m         b. Wrapper\n"
"\033[30m         c. Gestion du temps\n"
"\033[30m      III) Gestion des processus\n"
"\033[30m         a. Changement de contexte\n"
"\033[30m         b. Appels système\n"
"\033[30m         c. Ordonnancement\n"
"\033[31m    | IV) Pilotes\n"
"\033[31m    |    a. Clavier/Souris\n"
"\033[31m    |    b. Disquette\n"
"\033[30m      V) Entrées/sorties\n"
"\033[30m         a. Système de fichier FAT\n"
"\033[30m         b. Stdin/Stdout\n");
	*title = addButton(win,"IV) Pilotes");
}
void plan5(struct window_t *win, struct widget_t** txt, struct widget_t** title) {
	*txt = addTxt(win, 
"\n"
"\033[30m      I) Mémoire\n"
"\033[30m         a. Pagination\n"
"\033[30m         b. Allocation mémoire\n"
"\033[30m      II) Interruptions\n"
"\033[30m         a. IDT \n"
"\033[30m         b. Wrapper\n"
"\033[30m         c. Gestion du temps\n"
"\033[30m      III) Gestion des processus\n"
"\033[30m         a. Changement de contexte\n"
"\033[30m         b. Appels système\n"
"\033[30m         c. Ordonnancement\n"
"\033[30m      IV) Pilotes\n"
"\033[30m         a. Clavier/Souris\n"
"\033[30m         b. Disquette\n"
"\033[31m    | V) Entrées/sorties\n"
"\033[31m    |    a. Système de fichier FAT\n"
"\033[31m    |    b. Stdin/Stdout\n");
	*title = addButton(win,"V) Entrées/sorties");
}

/*
 * Slides gestion mémoire :
 */

void intro_pagination(struct window_t *win, struct widget_t** txt, struct widget_t** title) {
	*txt = addTxt(win, "\n\033[31mPourquoi la pagination ?\033[30m\n"
" \n"
"     - Sécurité (cloisonnement de la mémoire par processus, privilèges)\n"
" \n"
"     - Mémoire virtuelle : Possibilité d'utiliser plusieurs supports (swap)\n"
"                           Zones mémoires contigües virtuellement\n"
" \n"
"     - Concept important et très répandu : Intérêt pédagogique\n"
" \n"
"\033[31mRôle de la MMU (Memory Management Unit) :\n"
" \n"
"     - Traduction des adresses virtuelles en adresses physiques\n"
" \n"
"     - Levée d'une exception lorsqu'une adresse n'est pas mappée\n");
	*title = addButton(win, "Pagination : Son rôle");
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
	*title = addButton(win,"Pagination : Fonctionnement sur x86");
}

void pagination1(struct window_t *win, struct widget_t** txt, struct widget_t** title) {
	*txt = addTxt(win, "\n\033[31m1ère étape : Découpage de la mémoire en cadres de page\n"
" \n"
"   Objectif : Fournir des zones mémoires où l'on peut mapper des pages.\n"
" \n"
" \n"
"   Maintien de 2 listes : Cadres utilisés et Cadres libres.\n"
" \n"
"   Fonctions de reservation et de libération de cadres.\n");
	*title = addButton(win, "Pagination : Découpage de la mémoire physique");
}

void pagination2(struct window_t *win, struct widget_t** txt, struct widget_t** title) {
	*txt = addTxt(win, 
"\n\033[31m2ème étape : Pagination en Identity Mapping\n"
"\n"
"	Passage des adresses physiques aux adresses virtuelles délicate\n"
"\n"
"	Idée : Mapper les pages de façon à ce que :\n"
"             adresse virtuelle == adresse physique\n"
"\n"
"\n"
"	On ajoute une entrée dans le répertoire de tables de page pour pointer \n"
"	vers lui-même. => Pouvoir modifier les tables de page\n"
"\n"
"	On peut ainsi accéder à une table par l'adresse virtuelle :\n"
"					0xFFC00000 + 1024*index_page_table\n"
"\n"
"	Et au répertoire de tables de page par :\n"
"					0xFFFFF000\n");
	*title = addButton(win, "Pagination : Activation");
}


void pagination3(struct window_t *win, struct widget_t** txt, struct widget_t** title) {
	*txt = addTxt(win, 
"\n\033[31m3ème étape : Mise en place du VMM (Virtual Memory Manager)\n"
"\n"
"      Rôle : S'occuper de gérer des blocs de pages contigües en \n"
"          mémoire virtuelle.\n"
"\n"
"      Maintien de 2 listes : Blocs libres et Blocs utilisés.\n"
"\n"
"\n"
"\n"
"\033[31m4ème étape : Mise en place du kmalloc\n"
"\n"
"      Très similaire au VMM mais avec une gestion à l'octet.\n");
	*title = addButton(win, "Pagination");
}


void schema_vmm(struct window_t *win, struct widget_t** txt, struct widget_t** title) {
	*txt = addTxt(win, "                    ^                                   ^\n"
"                    |                                   |\n"
"  Fin de la zone    |                                   |\n"
"  gérée par     --->|-----------------------------------| -\n"
"  vmm.c (vmm_top)   |\033[43m         (zone utilisée)           \033[47m| | Bloc de\n"
"                    |\033[44m\033[34m-----------------------------------\033[47m\033[030m| | 2 pages\n"
"                    |-----------------------------------| -\n"
"                    | Zone pages virtuelles non mappées | | \n"
"                    |      à des pages physiques.       | | Bloc de\n"
"                    |           (zone libre)            | | 4 pages\n"
"                    |\033[44m\033[34m-----------------------------------\033[47m\033[030m| |\n"
"                    |-----------------------------------| -\n"
"                    |\033[43mZone pages virtuelles mappées à des\033[47m| | Bloc de    \n"
"                    |\033[43m pages physiques. (zone utilisée)  \033[47m| | 3 pages \n"
"                    |-----------------------------------| | (= 12 kio)\n"
"    Début de la     |\033[44m\033[37mStructure décrivant le bloc (16 o) \033[47m\033[030m| |\n"
"    zone gérée  --->|-----------------------------------| -\n"
"    par vmm.c       |       Noyau - Identity Mapped     |\n"
"                    |___________________________________|\n");
	*title = addButton(win,"Pagination : VMM");
}

void pagination_prob(struct window_t *win, struct widget_t** txt, struct widget_t** title) {
	*txt = addTxt(win, 
"\nSystème très complexe et avec beaucoup d'interdépendances :\n"
"                    Cadres de pages, VMM et kmalloc sont liés.\n"
"\n"
"Conséquences lourdes sur le reste du système d'exploitation :\n"
"                    kmalloc est nécessaire dans de très très nombreux cas.\n"
"\n"
"\n"
"Bugs difficiles à détecter et à corriger ! \n"
"   => Seul la MMU nous signale un problème.\n"
"   => Parfois un bug ne crée pas de plantage tout de suite.\n"
"   => Parfois un bug n'est pas mis en évidence avant longtemps.\n");
	*title = addButton(win,"Pagination : Difficultés");
}

void interruptions_conf(struct window_t *win, struct widget_t** txt, struct widget_t** title) {
	*txt = addTxt(win,
"\n\n     \033[31mUtilisations des interruptions:\033[30m\n"
"          => Communication avec les périphériques\n"
"          => Ordonnancement, timers...\n"
"          => Gestion des exceptions\n"
"\n\n"
"     \033[31mPour cela,\n" 
"     deux éléments à configurer:\033[30m\n"
"          => IDT\n"
"            (Interrupt Descriptor Table)\n"
"          => PIC\n"
"            (Programmable Interrupt Controler)\n");
*title = addButton(win,"Interruptions - Configuration");
}

void interruptions_idtpic(struct window_t *win, struct widget_t** txt, struct widget_t** title) {
	*txt = addTxt(win,
"\n\n     \033[31mRole de l'IDT:\033[30m\n" 
"          => Déterminer le type de gate pour chaque vecteur\n"
"            (interruption, exception, trap)\n"
"          => Contenir les ISR\n"
"             (Interrupt Service Routine)\n"      
"\n\n"
"     \033[31mRole du PIC:\033[30m\n"
"          => Permet de gérer les interruptions matérielles\n"
"          => Mapper ces interruption\n"
"          => Masquer ou non certaines interruptions\n");
*title = addButton(win,"IDT/PIC");
}

void interruptions_wrapper(struct window_t *win, struct widget_t** txt, struct widget_t** title) {
	*txt = addTxt(win,
"\n\n     \033[31mProblème:\033[30m\n"
"          Comment s'assurer que les ISR ne perturbent pas l'exécution\n"
"          du programme?\n\n"
"     \033[31mSolution:\033[30m\n"
"          Empiler les données importantes à chaque interruptions\n"
"          (registres et flags) et les dépiler ensuite\n\n"
"       => Utilisation de wrappers pour dispenser le programmeur à réaliser\n"
"          cette opération dans chaque ISR\n");
*title = addButton(win,"Wrapper");
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
"      \033[32mempilé par le  \033[34m    empilé après l'interruption     \033[31m empilé à        \n"
"      \033[32mmécanisme d'   \033[34m                                    \033[31m l'appel de      \n"
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
"\033[34mConfiguration control register\033[30m: spécification du débit de transfert\n"
"\n"
"                               \033[31mCommandes\033[30m\n"
"\n"
"\033[32mSpecify\033[30m         : donne au controleur des informations sur le lecteur\n"
"\033[32mWrite/Read data\033[30m : écrit/Lit un ou plusieurs secteurs sur la disquette\n"
"\033[32mRecalibrate\033[30m     : positionne les tetes en butée sur le cylindre 0\n"
"\033[32mSense interrupt\033[30m : récupère des informations sur une interruption\n"
"\033[32mSeek\033[30m            : déplace la tete de lecture/écriture à un cylindre donné\n"
"\033[32mVersion\033[30m         : détermine la version du controleur\n");
	*title = addButton(win,"Driver disquette");
}



void schema_fat(struct window_t *win, struct widget_t** txt, struct widget_t** title) {
	*txt = addTxt(win, 
"                       Partition FAT       \n"
"                \033[34m ________________________ \033[30m \n"
"                \033[34m|      Secteur de boot   |\033[30m<---*\n"
"                \033[34m|________________________|\033[30m    |\n"
"                \033[34m| File Allocation Tables |\033[30m    |\n"
"                \033[34m|________________________|\033[30m    |-- \033[34mZone réservée\n"
"                \033[34m|                        |\033[30m    |\n"
"                \033[34m|    Répertoire Racine   |\033[30m    |\n"
"                \033[34m|________________________|\033[30m<---*\n"
"                \033[32m|                        |\033[30m<---*\n"
"                \033[32m|                        |\033[30m    |\n"
"                \033[32m|       Fichiers et      |\033[30m    |\n"
"                \033[32m|    sous répertoires    |\033[30m    |\n"
"                \033[32m|                        |\033[30m    |-- \033[32mZone des données\n" 
"                \033[32m|                        |\033[30m    |\n"
"                \033[32m|                        |\033[30m    |\n"
"                \033[32m|                        |\033[30m    |\n"
"                \033[32m|________________________|\033[30m<---*\n");
	*title = addButton(win,"Système de fichiers FAT");
}

void diapo_fat(struct window_t *win, struct widget_t** txt, struct widget_t** title) {
	*txt = addTxt(win, 
"                     \033[31mInterprétation des valeurs de la FAT\033[30m\n"
"\n"
"\n"
"          \033[32m0x000\033[30m          Cluster vide\n"
"          \033[32m0x001\033[30m          Cluster réservé\n"
"          \033[32m0x002 - 0xFEF\033[30m  Pointeur vers le cluster suivant du fichier\n"
"          \033[32m0xFF0 - 0xFF6\033[30m  Valeurs réservées\n"
"          \033[32m0xFF7\033[30m          Cluster défectueux\n"
"          \033[32m0xFF8 - 0xFFF\033[30m  Dernier cluster d'un fichier\n"
"\n"
"\n"
"                        \033[31mEntrée répertoire - 32 octets\033[30m\n"
"\n"
"                          Nom | Extension | \033[34mAttributs\033[30m\n"
"                   Date/Heure création | Date dernier accès\n"
"                       Date/Heure dernières modifications\n"
"                    \033[34mNo du premier cluster\033[30m | \033[34mTaille en octet\033[30m\n");

	*title = addButton(win,"Système de fichiers FAT");
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
	*title = addButton(win,"Entrées / Sorties");
}

void souris(struct window_t *win, struct widget_t** txt, struct widget_t** title) {
	*txt = addTxt(win, 
"           \n"
"         Fonctionnement PS/2 :\n"
"           \n"
"           \n"
"            \033[40m\033[37m --- \033[30m\033[47m   <Port[0x60]   \033[40m\033[37m ---- \033[30m\033[47m         \033[37m\033[44m --------------- \033[47m\033[30m\n"
"            \033[40m\033[37m|CPU|\033[30m\033[47m<--------------->\033[40m\033[37m|PS/2|\033[30m\033[47m\033[34m<------->\033[37m\033[44m|    premier    |\033[47m\033[30m\n"
"            \033[40m\033[37m --- \033[30m\033[47m   Port[0x64]>   \033[40m\033[37m|KBC |\033[30m\033[47m\033[31m<---+    \033[37m\033[44m|connecteur PS/2|\033[47m\033[30m\n"
"              ^                   \033[40m\033[37m ---- \033[31m\033[47m    |    \033[37m\033[44m --------------- \033[47m\033[30m\n"
"              |                   \033[34m | \033[31m |     |\n"
"           \033[44m\033[37m ---- \033[47m\033[34m     IRQ1         | \033[31m |     |\n"
"           \033[44m\033[37m|PIC1|\033[47m\033[34m<-----------------+ \033[31m |     |\n"
"           \033[44m\033[37m ---- \033[47m                    \033[31m |     |    \033[37m\033[41m --------------- \033[47m\033[30m\n"
"              ^                      \033[31m |     +--->\033[37m\033[41m|    second     |\033[47m\033[30m\n"
"              |                      \033[31m |          \033[37m\033[41m|connecteur PS/2|\033[47m\033[30m\n"
"           \033[41m\033[37m ---- \033[47m\033[31m     IRQ12           |          \033[37m\033[41m --------------- \033[47m\033[30m\n"
"           \033[41m\033[37m|PIC2|\033[47m\033[31m<--------------------+\n"
"           \033[41m\033[37m ---- \033[47m\033[30m\n"
" \n");
	*title = addButton(win,"Clavier/Souris");
}

void slide_time(struct window_t *win, struct widget_t** txt, struct widget_t** title) {
	*txt = addTxt(win, 
"    \033[31m  Récupération de la date :\033[30m\n"
"          communication avec la Real Time Clock (RTC)\n"
"          sur les ports 0x70 et 0x71\n"
"      \n"
"  \033[31m    Fonctionnement du PIT I8254 (Programmable Interval Timer) :\033[30m\n"
"      \n"
"                     _   _    \033[40m\033[37m ---------- \033[30m\033[47m\n"
"                   _| |_|  -> \033[40m\033[37m| Compteur |\033[30m\033[47m -> IRQ0\n"
"                              \033[40m\033[37m ---------- \033[30m\033[47m\n"
"                                   ^\n"
"                                   |\n"
"                               port 0x40\n"
"      \n"
"   \033[31m   Echelles de temps :\033[30m\n"
"         ticks -> secondes -> dates\n"
"      \n"
"  \033[31m    Planification d'évènements :\033[30m\n"
"         sleep, ordonnancement ...\n"
" \n");
	*title = addButton(win,"Gestion du temps");
}

void conclusion(struct window_t *win, struct widget_t** txt, struct widget_t** title) {
	*txt = addTxt(win, 
"\n"
"\033[31m             Passage à une architecture micro-kernel\n"
"\033[30m                   => refonte profonde de l'architecture\n"
"\n"
"\n"
"\033[31m             Affichage graphique\n"
"\033[30m                   => utilisation du mode vm86\n"
"\n"
"\n"
"\033[31m             Communication entre les processus\n"
"\033[30m                   => sémaphores\n"
"\033[30m                   => mémoire partagée\n"
"\033[30m                   => pipes\n"
"\n"
"\n"
"\033[31m             Support du reseau\n"
"\033[30m                   => driver carte réseau\n"
"\033[30m                   => support des différentes couches réseau\n"
"\n");
	*title = addButton(win,"Perspectives pour la suite");
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
	interruptions_idtpic(win,&txt[i],&title[i]);
	i++;
	interruptions_wrapper(win,&txt[i],&title[i]);
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
		setWidgetProperties(title[i], 7, 1, 3, 66, 6, 0);
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

