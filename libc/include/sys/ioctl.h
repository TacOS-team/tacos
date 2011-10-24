#ifndef _IOCTL_H
#define _IOCTL_H

#define TCGETS 1
#define TCSETS 2

int ioctl(int d, unsigned int request, void *data);

#endif
