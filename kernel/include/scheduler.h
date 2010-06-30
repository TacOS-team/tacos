#ifndef SCHEDULER_H
#define SCHEDULER_H

void init_scheduler(int Quantum);
void start_scheduler();

void* sys_exec(paddr_t prog, char* name, uint32_t unused __attribute__ ((unused)));
void exec(paddr_t prog, char* name);

void* sys_sleep( uint32_t delay,uint32_t unused2 __attribute__ ((unused)), uint32_t unused3 __attribute__ ((unused)));

#endif //SCHEDULER_H
