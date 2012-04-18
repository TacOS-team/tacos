#ifndef __LIBTEST_H__
#define __LIBTEST_H__

#include <stdlib.h>
#include <cstdio>

#ifndef NBITER
#define NBITER 1000
#endif

#define ERROR(mess) printf("ERREUR ligne %d : %s\n", __LINE__, mess);

#endif// __LIBTEST_H__

