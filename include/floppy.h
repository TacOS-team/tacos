#ifndef _FLOPPY_H_
#define _FLOPPY_H_

void floppy_detect_drives();
int init_floppy();
uint8_t floppy_get_version();
int floppy_cylinder(int base, int cylinder, int io_dir);




#endif
