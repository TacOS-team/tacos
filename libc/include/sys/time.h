#ifndef _SYS_TIME_H_
#define _SYS_TIME_H_

#include <sys/cdefs.h>

__BEGIN_DECLS

#include <time.h>

int gettimeofday(struct timeval *tv, void *tz_unused);

__END_DECLS

#endif
