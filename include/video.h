#include <types.h>

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
