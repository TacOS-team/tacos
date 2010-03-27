#ifndef _MOUSE_H_
#define _MOUSE_H_

void mouseInit();
void mouseInterrupt(int id);
void getMouseCoord(int* x, int* y);
int getMouseBtn(int btnId);

#endif

