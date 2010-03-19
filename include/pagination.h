#ifndef _PAGINATION_H_
#define _PAGINATION_H_

// PDE : Référence l'adresse d'une table de pages. (cf doc intel p125)
struct  page_directory_entry {
	uint32_t present:1; // doit être à 1 pour référencer une table de pages
	uint32_t r_w:1; // 0 writes non autorisés sur la table de page référencée par cette entrée (dépend de CPL et CR0.WP)
	uint32_t u_s:1; // User/supervisor. Si 0 alors accès avec CPL=3 non autorisés à la table de page référencée par cette entrée.
	uint32_t pwt:1; // Page-level write-through ?? voir section 4.9 de la doc intel
	uint32_t pcd:1; // Page-level cache disable, voir section 4.9 de la doc intel
	uint32_t a:1; // Accessed : indique si un soft a accedé à la table de pages référencée par cette entrée
	uint32_t ign1:1; // ignoré
	uint32_t ps:1; // Page size : doit être à 0 sinon c'est qu'on map une page de 4Mio
	uint32_t ign2:4; // ignoré
	uint32_t page_table_addr:20; // adresse de la table de page (alignée sur 4kio donc que les 20 bits de poids fort)
} __attribute__ ((packed));

// PTE : Référence l'adresse d'une page
struct page_table_entry {
	uint32_t present:1; // doit être à 1 pour référencer une page
	uint32_t r_w:1; // 0 writes non autorisés sur la page référencée par cette entrée (dépend de CPL et CR0.WP)
	uint32_t u_s:1; // User/supervisor. Si 0 alors accès avec CPL=3 non autorisés à la page référencée par cette entrée.
	uint32_t pwt:1; // Page-level write-through ?? voir section 4.9 de la doc intel
	uint32_t pcd:1; // Page-level cache disable, voir section 4.9 de la doc intel
	uint32_t a:1; // Accessed : indique si un soft a accedé à la page référencée par cette entrée
	uint32_t d:1; // Dirty : indique si un soft a écrit sur la page
	uint32_t pat:1; // si pat activé alors ça détermine le type de mémoire utilisée pour accéder à la page. Sinon faut le mettre à 0.
	uint32_t g:1; // ?? cf doc.
	uint32_t ignored:3; // ignoré
	uint32_t page_addr:20; // adresse de la page (alignée sur 4kio donc que les 20 bits de poids fort)
} __attribute__ ((packed));


void pagination_setup();
void pagination_init_page_directory(struct page_directory_entry * pd);
void pagination_load_page_directory(struct page_directory_entry * pd);

#endif
