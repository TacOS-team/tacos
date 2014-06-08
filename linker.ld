ENTRY (start)

SECTIONS{
	/* Notre kernel est chargé à cette adresse (juste après la partie réservée au BIOS) : */
    . = 0x00100000;
	/* Débug chargement (load_addr) */
	 __b_load = .;

    .text  ALIGN (0x500) :{
        *(.__mbHeader)
        *(.text)
    }


    .rodata ALIGN (0x1000) : {
        *(.rodata)
    }

    .data ALIGN (0x1000) : {
        *(.data)
    }

	/* On marque où est la fin du chargement des data (load_end_addr) */
	 __e_load = .;

    .bss : {
        sbss = .;
        *(COMMON)
        *(.bss)
        ebss = .;
    }

	/* On marque où se termine le kernel (bss_end_addr) */
	 __e_kernel = .;
	 
}

