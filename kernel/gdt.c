#include <types.h>
#include "memory.h"
#include "gdt.h"

static tss_t default_tss;

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

	/* Segment pour le code Kernel (privilege = 0)*/
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

	/* Segment pour les data Kernel (privilege = 0) */
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
	
	/* Segment pour le code Utilisateur (privilege = 3)*/
	[3] = (struct x86_segment_descriptor){
		.segment_limit_15_0 = 0xFFFF,
		.segment_limit_19_16 = 0xF,
		.base_address_15_0 = 0,
		.base_address_23_16 = 0,
		.base_address_31_24 = 0,
		.segment_type = 0xB,
		.descriptor_type = 1,
		.dpl = 3,
		.present = 1,
		.available = 0,
		.zero = 0,
		.operation_size = 1,
		.granularity = 1
	},

	/* Segment pour les data Utilisateur (privilege = 0) */
	[4] = (struct x86_segment_descriptor){
		.segment_limit_15_0 = 0xFFFF,
		.segment_limit_19_16 = 0xF,
		.base_address_15_0 = 0,
		.base_address_23_16 = 0,
		.base_address_31_24 = 0,
		.segment_type = 0x3,
		.descriptor_type = 1,
		.dpl = 3,
		.present = 1,
		.available = 0,
		.zero = 0,
		.operation_size = 1,
		.granularity = 1
	},
	
	
	/* Descripteur de la TSS (Cf doc Intel v3 6.2.2) */
	[5] = (struct x86_segment_descriptor){
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

	gdt[1].segment_limit_15_0 = (ram_size / PAGE_SIZE + 1) & 0xffff;
	gdt[1].segment_limit_19_16 = ((ram_size / PAGE_SIZE + 1) >> 16) & 0xf;
	gdt[2].segment_limit_15_0 = (ram_size / PAGE_SIZE + 1) & 0xffff;
	gdt[2].segment_limit_19_16 = ((ram_size / PAGE_SIZE + 1) >> 16) & 0xf;
	
	gdt[3].segment_limit_15_0 = (ram_size / PAGE_SIZE + 1) & 0xffff;
	gdt[3].segment_limit_19_16 = ((ram_size / PAGE_SIZE + 1) >> 16) & 0xf;
	gdt[4].segment_limit_15_0 = (ram_size / PAGE_SIZE + 1) & 0xffff;
	gdt[4].segment_limit_19_16 = ((ram_size / PAGE_SIZE + 1) >> 16) & 0xf;

	/* Initialisation du descripteur de TSS dans la GDT */
	gdt[5].base_address_15_0 = ((uint32_t)(&default_tss) & 0xffff);
	gdt[5].base_address_23_16 = (((uint32_t)(&default_tss) >> 16) & 0xff);
	gdt[5].base_address_31_24 = (((uint32_t)(&default_tss) >> 24) & 0xff);
	

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

void init_tss(uint32_t stack_address)
{
	default_tss.debug_flag = 0x00;
	default_tss.io_map = 0x00;
	default_tss.esp0 = stack_address;
	default_tss.ss0 = 0x08;
	
	asm volatile(
		"mov $0x28, %ax\n\t"
		"ltr %ax");
}

tss_t* get_default_tss()
{
	return &default_tss;
}


