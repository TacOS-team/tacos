#ifndef SCHEDULER_H
#define SCHEDULER_H

void init_scheduler(int Quantum);
void start_scheduler();

void* sys_exec(paddr_t prog, char* name, uint32_t unused __attribute__ ((unused)));
void exec(paddr_t prog, char* name);

#endif //SCHEDULER_H
