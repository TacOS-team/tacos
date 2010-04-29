#ifndef _SHELL_UTILS_H
#define _SHELL_UTILS_H

void add_builtin_cmd(paddr_t func, char* name);
int exec_builtin_cmd(char* name);
void show_builtin_cmd();

#endif
