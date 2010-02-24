#ifndef SCHEDULER_H
#define SCHEDULER_H

void init_scheduler();

int add_process(paddr_t prog, uint32_t argc, uint8_t** argv);

#endif //SCHEDULER_H
