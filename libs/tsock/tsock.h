#ifndef TSOCK_H
#define TSOCK_H

#include <sys/cdefs.h>

__BEGIN_DECLS

#include <sys/types.h>

int tsock_listen(const char *path);
    
int tsock_connect(const char *path);

int tsock_accept(int tsock);

ssize_t tsock_read(int tsock, void *buffer, size_t len);

ssize_t tsock_write(int tsock, void *buffer, size_t len);

void tsock_close(int tsock);

int tsock_set_nonblocking(int tsock);

int tsock_set_blocking(int tsock);

__END_DECLS

#endif
