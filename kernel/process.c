#include <types.h>
#include <stdlib.h>
#include <stdio.h>

#include "process.h"

struct cpu_cxt {

    uint16_t  gs;
    uint16_t  fs;
    uint16_t  ds;
    uint16_t  ss;
    uint16_t  cs; 
    uint16_t  alignement; //inutilisé

    uint32_t  edi;
    uint32_t  esi;
    uint32_t  ebp;
    uint32_t  esp;
    uint32_t  ebx;
    uint32_t  edx;
    uint32_t  ecx;
    uint32_t  eax;
    uint32_t  eflags;
    uint32_t  eip;

} __attribute__((packed));

int cup_ctxt_init (paddr_t* pStack, paddr_t* pFunct) {
  
  paddr_t CurrentSatck;
  asm volatile (\
   "movl %%esp, %0;\
    movl %1, %%esp;\
    pushl %2;\
    pushl %%ebp;\
    pushl $0;\
    pushw  %%gs;\
    pushw  %%fs;\
    pushw  %%ds;\
    pushw  %%ss;\
    pushl  %%edi;\
    pushl  %%esi;\
    pushl  %%ebp;\
    pushl  $0;\
    pushl  $0;\
    pushl  $0;\
    pushl  $0;\
    movl %%esp, %1;\
    movl %0, %%esp;"\
    ::\
    "m"(CurrentSatck),\
    "m"(*pStack),\
    "m"(pFunct)\
  );

  return 0;

}

void cpu_ctxt_switch(paddr_t* pOldStack, paddr_t* pNewSatck) {

  // A ce moment la (Appel de la function cpu_ctxt_switch) 
  // l'eip ext automatique stocké dans la pile comme adresse de retour
  // dans la pile ou l'on est c.a.d dans la pile de l'ancien processus
  // On stocke alors les flags et touts les registres cpu dans cette meme pile
  asm volatile (\
   "pushf;\
    pushw  %gs;\
    pushw  %fs;\
    pushw  %ds;\
    pushw  %ss;\
    pushl  %edi;\
    pushl  %esi;\
    pushl  %ebp;\
    pushl  %ebx;\
    pushl  %edx;\
    pushl  %ecx;\
    pushl  %eax;"\
  );
  // On change l'esp pour qu'il pointe sur la pile du nouveau processus
  asm volatile (\
   "movl %%esp, %0;\
    movl %1, %%esp;"\
    ::\
    "m"(*pOldStack),\
    "m"(*pNewSatck)\
  );
  // On retablie le nouveau contexte précédement sauvegardé dans la pile
  asm volatile (\
   "popl  %eax;\
    popl  %ecx;\
    popl  %edx;\
    popl  %ebx;\
    popl  %ebp;\
    popl  %esi;\
    popl  %edi;\
    popw  %ss;\
    popw  %ds;\
    popw  %fs;\
    popw  %gs;\
    popfl;"\
  );
  // L'instruction ret remet donc l'eip précedment sauvegardé
  asm volatile (" ret;");

}


