#include <types.h>
#include <stdio.h>
#include "mempage.h"

struct physical_page_descr {
	/* Contient bien sûr l'adresse physique de la zone mémoire ! */
	paddr_t addr;

	/* Cet élément fera parti d'un liste doublement chainée (page utilisée ou libre) */
	struct physical_page_descr *next;
	struct physical_page_descr *prev;
};

static struct physical_page_descr *used_pages = NULL;
static struct physical_page_descr *free_pages = NULL;

/* C'est les marqueurs qui me disent où commence le kernel et où il se fini en mémoire. */
extern char _start, __e_kernel;

static struct physical_page_descr * phys_page_descr_array;

/**
 * Retourne l'adresse de la page en arrondissant à l'inférieur.
 */
static paddr_t mempage_align_page_inf(paddr_t value) {
	return value - value%PAGE_SIZE;
};

/**
 * Retourne l'adresse de la page en arrondissant au supérieur.
 */
static paddr_t mempage_align_page_sup(paddr_t value) {
	if (value % PAGE_SIZE == 0) {
		return value;
	}
	value += PAGE_SIZE;
	return value - value%PAGE_SIZE;
};
	
/**
 * Affiche les pages utilisées.
 */
void mempage_print_used_pages() {
	struct physical_page_descr *p;

	printf("Used pages : ");

	p = used_pages;
	while (p != NULL) {
		printf("%x ", p->addr);
		p = p->next;
	}
	printf("\n");
}

/**
 * Affiche les pages disponibles.
 */
void mempage_print_free_pages() {
	struct physical_page_descr *p;

	printf("Free pages : ");

	p = free_pages;
	while (p != NULL) {
		printf("%x ", p->addr);
		p = p->next;
	}
	printf("\n");
}

/**
 * Découpage de la mémoire physique en taille fixe. Pour l'instant il n'y a pas de mémoire 
 * virtuelle, c'est donc juste une association linéaire avec autant de pages
 * que de cadres.
 */
void mempage_setup(size_t ram_size) {
	ram_size = mempage_align_page_inf(ram_size); /* On abandonne un petit bout de mémoire pour avoir que des pages completes */

	/* On place le tableau de pages juste après la fin du kernel. */
	phys_page_descr_array = (struct physical_page_descr*) mempage_align_page_sup((size_t)(& __e_kernel));

	size_t phys_mem_base = PAGE_SIZE; /* La page d'adresse 0 est réservée pour les erreurs. (c'est un choix arbitraire) */
	size_t phys_mem_top = ram_size;

	/* Maintenant on va récupérer les pages qui chevauchent l'emplacement où est le
	 * kernel ainsi que la table qui contient les pages. On va les marquer utilisées. */
	paddr_t kernel_base = mempage_align_page_inf((paddr_t)(& _start));
	uint32_t n_pages = ram_size / PAGE_SIZE; /* ram_size est un multiple de PAGE_SIZE ! */
	paddr_t kernel_top = (paddr_t)phys_page_descr_array + mempage_align_page_sup(n_pages * sizeof(struct physical_page_descr));

	struct physical_page_descr *phys_page_descr = (struct physical_page_descr*) phys_page_descr_array;
	paddr_t phys_page_addr = phys_mem_base; /* Adresse d'une page */

	/* On va initialiser le tableau de descripteur de pages. */
	while (phys_page_addr < phys_mem_top) {
		phys_page_descr->addr = phys_page_addr;

		/* Si la zone mémoire est déjà utilisée par le kernel ou par le BIOS alors on marque les pages comme 
		 * étant déjà utilisées pour ne pas les attribuer. */
		if ((phys_page_addr >= BIOS_RESERVED_BASE && phys_page_addr < BIOS_RESERVED_TOP)
				|| (phys_page_addr >= kernel_base && phys_page_addr < kernel_top)) {
			/* TODO : Ajouter des méthodes de gestion d'une liste doublement chainée... */
			phys_page_descr->next = used_pages;
			phys_page_descr->prev = NULL;
			if (used_pages != NULL) {
				used_pages->prev = phys_page_descr;
			}
			used_pages = phys_page_descr;
		} else {
			phys_page_descr->next = free_pages;
			phys_page_descr->prev = NULL;
			if (free_pages != NULL) {
				free_pages->prev = phys_page_descr;
			}
			free_pages = phys_page_descr;
		}

		/* On passe à la page suivante ! */
		phys_page_descr++;
		phys_page_addr += PAGE_SIZE;
	}
}

paddr_t mempage_reserve_page()
{
  struct physical_page_descr *p = free_pages;
 
  // pop it from free page stack
  free_pages = free_pages->next;
  if(free_pages != NULL)
    free_pages->prev = NULL;
 
  // put the new reserved on used page stack
	used_pages->prev = p;
  p->next = used_pages;
  used_pages = p;
  p->prev = NULL;

  return p->addr;
}

void mempage_free_page(paddr_t addr)
{
  int found = 0;
	struct physical_page_descr *p;

	p = used_pages;
	while (p != NULL)
  {
    if(p->addr == addr)
    {
      found = 1;
      if(p->next != NULL)
        p->next->prev = p->prev;
      if(p->prev != NULL)
        p->prev->next = p->next;

      break;
    }
	}
  
  if(!found)
    return -1;

  used_pages = used_pages->next;

  if(free_pages != NULL) 
    free_pages->prev = p;
  p->prev = NULL;
  p->next = free_pages;
  free_pages = p;
}
