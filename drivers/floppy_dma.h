#ifndef _FLOPPY_DMA_H_
#define _FLOPPY_DMA_H_

typedef enum {floppy_write = 1, floppy_read = 2} floppy_io;

void floppy_dma_init(floppy_io io_dir);
void floppy_read_sector(int cylinder, int head, int sector, char* buffer);
void floppy_write_sector(int cylinder, int head, int sector, char* buffer);



#endif
