#ifndef _SHELL_UTILS_H
#define _SHELL_UTILS_H

typedef int (*func_ptr)(int argc, char **argv);

void add_builtin_cmd(func_ptr func, char* name);
int exec_builtin_cmd(char* name);
void show_builtin_cmd();

#endif
