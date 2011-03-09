#ifndef _MODULE_H
#define _MODULE_H

#include <type.h>

#define MODULE_NAME(X) char __modname[] __attribute__ ((section(".modname"))) = X
#define MODULE_VERSION(X) char __modversion[] __attribute__ ((section(".modversion"))) = X
#define MODULE_LOAD(X) void*  __modinit __attribute__ ((section(".modload"))) = X
#define MODULE_UNLOAD(X) void*  __modclean __attribute__ ((section(".modunload"))) = X;

typedef struct {
	
	char* name;		/* Name loaded from .modname section */
	
	char* version; 	/* Version loaded from .modversion section */
	
	paddr_t load_handler; 	/* Address of the module_load function, loaded from .modload section */
	
	paddr_t unload_handler;	/* Address of the module_unload function, loaded from .modunload section */
	
	paddr_t load_addr;	/* Address where the module is loaded */
	
}module_info_t;

#endif /* _MODULE_H */
