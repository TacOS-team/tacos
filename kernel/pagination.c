typedef struct {
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
} __attribute__ ((packed)) page_directory_entry;
