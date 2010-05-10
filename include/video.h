#include <types.h>

#define BLACK         0x0
#define BLUE          0x1
#define GREEN         0x2
#define CYAN          0x3
#define RED           0x4
#define MAGENTA       0x5
#define BROWN         0x6
#define LIGHT_GRAY    0x7
#define DARK_GRAY     0x8
#define LIGHT_BLUE    0x9
#define LIGHT_GREEN   0xA
#define LIGHT_CYAN    0xB
#define LIGHT_RED     0xC
#define LIGHT_MAGENTA 0xD
#define YELLOW        0xE
#define WHITE         0xF

void disableCursor();
void cls (void);
void newline();
void enableFinnouMode(int enable);
void set_attribute(uint8_t background, uint8_t foreground);
void set_attribute_position(uint8_t background, uint8_t foreground, int x, int y);
void reset_attribute();
void switchDebugBuffer();
void switchStandardBuffer();
void kputchar (char c);
void init_video();
