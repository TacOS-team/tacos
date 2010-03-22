#ifndef SCHEDULER_H
#define SCHEDULER_H

void init_scheduler(int Quantum);
void start_scheduler();
int add_process(process_t new_proc);

#endif //SCHEDULER_H
