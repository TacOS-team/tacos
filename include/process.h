#ifndef _PROCESS_H_
#define _PROCESS_H_


#include <types.h>


struct cpu_ctxt;
int cup_ctxt_init (paddr_t* pStack, paddr_t* pFunct);
void cpu_ctxt_switch(paddr_t* pOldStack, paddr_t* pNewSatck);

#endif
