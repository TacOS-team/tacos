#include <types.h>
#include "memory.h"

/*
 * Segment Descriptor (cf doc intel v3. 3.4.3)
 */
struct x86_segment_descriptor
{
	/* Double mot de poids faible */
	uint16_t segment_limit_15_0; /* Segment Limit 15:00. Spécifie la taille du segment. 
											  Le proc concatene les 2 champs segment limit pour 
											  former une valeur de 20 bits. */
	uint16_t base_address_15_0;  /* Défini la position de l'octet 0 du segment 
										 	  dans les 4Gio de l'espace adressable linéaire */

	/* Double mot de poids fort */
	uint8_t base_address_23_16; /* Suite de la base address. */
	uint8_t segment_type:4; /* Cf 3.4.3.1 de la doc intel ! */
	uint8_t descriptor_type:1; /* 0 = system segment, 1 = code ou data segment */
	uint8_t dpl:2; /* Descriptor Privilege Level. 0 à 3. 0 pour le kernel et 3 pour les applis */
	uint8_t present:1; /* Indique si le segment est présent en mémoire (1) ou non (0) */
	uint8_t segment_limit_19_16:4; /* Suite du segment limit. */
	uint8_t available:1; /* Un bit disponible pour nous... */
	uint8_t zero:1; /* Bit réservé qui doit prendre la valeur 0. */
	uint8_t operation_size:1; /* 0 = 16bits, 1 = 32bits */
	uint8_t granularity:1; /* Granularité : Détermine l'échele du segment limit. 0 : c'est des bytes, 1 : c'est par 4kio */
	uint8_t base_address_31_24; /* Fin de la base address. Soit 32 bits ce qui correspond aux 4Gio adressables. */

} __attribute__ ((packed, aligned (8)));

struct tss {
    uint16_t    previous_task, __previous_task_unused;
    uint32_t    esp0;
    uint16_t    ss0, __ss0_unused;
    uint32_t    esp1;
    uint16_t    ss1, __ss1_unused;
    uint32_t    esp2;
    uint16_t    ss2, __ss2_unused;
    uint32_t    cr3;
    uint32_t    eip, eflags, eax, ecx, edx, ebx, esp, ebp, esi, edi;
    uint16_t    es, __es_unused;
    uint16_t    cs, __cs_unused;
    uint16_t    ss, __ss_unused;
    uint16_t    ds, __ds_unused;
    uint16_t    fs, __fs_unused;
    uint16_t    gs, __gs_unused;
    uint16_t    ldt_selector, __ldt_sel_unused;
    uint16_t    debug_flag, io_map;
} __attribute__ ((packed));

struct tss default_tss;

/*
    default_tss.debug_flag = 0x00;
    default_tss.io_map = 0x00;
    default_tss.esp0 = 0x1FFF0;
    default_tss.ss0 = 0x18;
*/
/*
 * Registre gdt
 */
struct x86_gdt_register {
	/* Contient la taille */
	uint16_t limit; /* Attention, un descripteur de segment faisant 8 octets, 
							 la limite doit être un multiple de 8, moins 1 (8N-1). */

	/* Et l'adresse où sont les descripteurs de segment. */
	uint32_t	base_addr;
} __attribute__((packed, aligned(8)));

static struct x86_segment_descriptor gdt[] = {
	/* Le premier descripteur n'est pas utilisé par le processeur. On l'appelle "null descriptor".
	 * Il génère une exception lorsqu'on essaye d'accéder à la mémoire en l'utilisant. */
	[0] = (struct x86_segment_descriptor){0,0,0,0,0,0,0,0,0,0,0,0,0},

	/* Segment pour le code */
	[1] = (struct x86_segment_descriptor){
		.segment_limit_15_0 = 0xFFFF,
		.segment_limit_19_16 = 0xF,
		.base_address_15_0 = 0,
		.base_address_23_16 = 0,
		.base_address_31_24 = 0,
		.segment_type = 0xB,
		.descriptor_type = 1,
		.dpl = 0,
		.present = 1,
		.available = 0,
		.zero = 0,
		.operation_size = 1,
		.granularity = 1
	},

	/* Segment pour les data */
	[2] = (struct x86_segment_descriptor){
		.segment_limit_15_0 = 0xFFFF,
		.segment_limit_19_16 = 0xF,
		.base_address_15_0 = 0,
		.base_address_23_16 = 0,
		.base_address_31_24 = 0,
		.segment_type = 0x3,
		.descriptor_type = 1,
		.dpl = 0,
		.present = 1,
		.available = 0,
		.zero = 0,
		.operation_size = 1,
		.granularity = 1
	},
	
	/* Descripteur de la TSS (Cf doc Intel v3 6.2.2) */
	[3] = (struct x86_segment_descriptor){
		.segment_limit_15_0 = (sizeof(default_tss) & 0xffff),
		.segment_limit_19_16 = ((sizeof(default_tss) >> 16) & 0xf),
		.base_address_15_0 = 0,
		.base_address_23_16 = 0,
		.base_address_31_24 = 0,
		.segment_type = 0x9,  // Pas sûr pour le busy flag 
		.descriptor_type = 0,
		.dpl = 0,
		.present = 1,
		.available = 1, 
		.zero = 0,
		.operation_size = 1,
		.granularity = 1
	}

	/* Peut être aussi pour la stack ? à voir... */
};

/*
 * Configuration de la gdt.
 */
void gdt_setup(size_t ram_size) {
	struct x86_gdt_register gdtr;

	/* Adresse de la GDT */
	gdtr.base_addr = (uint32_t) gdt;

	/* Offset/Taille de la gdt */
	gdtr.limit     = sizeof(gdt) - 1;
	
	/* On utilise des segments dont la limite correspond à la mémoire physique ! */

	/*
	gdt[1].segment_limit_15_0 = (ram_size / PAGE_SIZE + 1) & 0xffff;
	gdt[1].segment_limit_19_16 = ((ram_size / PAGE_SIZE + 1) >> 16) & 0xf;
	gdt[2].segment_limit_15_0 = (ram_size / PAGE_SIZE + 1) & 0xffff;
	gdt[2].segment_limit_19_16 = ((ram_size / PAGE_SIZE + 1) >> 16) & 0xf;
	*/

	/* Initialisation du descripteur de TSS dans la GDT */
	gdt[3].base_address_15_0 = ((uint32_t)(&default_tss) & 0xffff);
	gdt[3].base_address_23_16 = (((uint32_t)(&default_tss) >> 16) & 0xff);
	gdt[3].base_address_31_24 = (((uint32_t)(&default_tss) >> 24) & 0xff);

	/* On commit la gdt avec lgdt. Puis on fait un saut long dans le segment code où on va ensuite reloader les registres avec le segment data.  */
	asm volatile ("lgdt %0          \n\
						ljmp $0x08,$1f \n\
						1: \n\
						movw $0x10, %%ax    \n\
						movw %%ax,  %%ss \n\
						movw %%ax,  %%ds \n\
						movw %%ax,  %%es \n\
						movw %%ax,  %%fs \n\
						movw %%ax,  %%gs"
			:
			:"m"(gdtr)
			:"memory","eax"); // memory pour lui dire d'appliquer direct les modifs sur eax.
}
