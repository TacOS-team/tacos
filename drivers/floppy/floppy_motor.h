#ifndef _FLOPPY_MOTOR_H_
#define _FLOPPY_MOTOR_H_

typedef enum { OFF = 0, ON = 1 } floppy_motor_state ;
void floppy_motor(floppy_motor_state new_state);

#endif
