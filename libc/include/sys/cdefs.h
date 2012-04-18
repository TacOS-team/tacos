#ifndef _SYS_CDEFS_H_
#define _SYS_CDEFS_H_

/* C++ needs to know that types and declarations are C, not C++. */
#ifdef  __cplusplus
# define __BEGIN_DECLS  extern "C" {
# define __END_DECLS  }
#else
# define __BEGIN_DECLS
# define __END_DECLS
#endif

#endif
