#ifndef WIDGET_H
#define WIDGET_H

#define MAX_WIDGETS 100

enum widget_type
{
	BUTTON,
	TXT
};

struct window_t;

struct widget_t
{
	char visible;
	void* adv;
	enum widget_type type;
	uint8_t x;
	uint8_t y;
	uint8_t w;
	uint8_t h;
	uint8_t bg;
	uint8_t fg;
	struct window_t* father;
	void (*onClick)(struct widget_t*, int, int);
};

struct window_t
{
	int nb_widgets;
	uint8_t bg;
	uint8_t cursor;
	struct widget_t* widgets[MAX_WIDGETS];
};


#endif //WIDGET_H

