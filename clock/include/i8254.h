#ifndef _I8254_H_
#define _I8254_H_

#include <types.h>

// Min Max interruption frequency
#define I8254_MIN_FREQ 2
#define I8254_MAX_FREQ 1193182

void i8254_init(uint16_t freq);

#endif

