#ifndef _FLOPPY_H_
#define _FLOPPY_H_

void floppy_detect_drives();
int init_floppy();
uint8_t floppy_get_version();
void floppy_read_sector(int cylinder, int head, int sector, char* buffer);
void floppy_write_sector(int cylinder, int head, int sector, char* buffer);




#endif
