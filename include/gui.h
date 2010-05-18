#ifndef GUI_H
#define GUI_H

#include <types.h>

struct window_t;
struct widget_t;

struct window_t* createWindow(uint8_t bg, uint8_t cursor);
void runWindow(struct window_t* win);
int freeWindow(struct window_t* win);

struct widget_t* addButton(struct window_t* win, const char* title);
struct widget_t* addTxt(struct window_t* win, const char* txt);
void setWidgetProperties(struct widget_t* wdg, 
									uint8_t x, uint8_t y,
									uint8_t h, uint8_t w,
									uint8_t bg, uint8_t fg);
void setOnClick(struct widget_t* wdg, void (*onClick)(struct widget_t*,int,int));

#endif //GUI_H

