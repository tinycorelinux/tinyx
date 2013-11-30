#ifdef HAVE_DIX_CONFIG_H
#include <dix-config.h>
#endif

#define XTRANS_TRANSPORT_C

#define TRANS_REOPEN
#define TRANS_SERVER
#define XSERV_t

//#ifdef XSERV_t
#include "os.h"
//#else
#include <stdlib.h>
#define xalloc(_size)		malloc(_size)
#define xcalloc(_num,_size)	calloc(_num,_size)
#define xrealloc(_ptr,_size)	realloc(_ptr,_size)
#define xfree(_ptr)		free(_ptr)
//#endif

#include <X11/Xtrans/Xtransint.h>

#ifdef LOCALCONN
#include <X11/Xtrans/Xtranslcl.c>
#endif
#if defined(TCPCONN) || defined(UNIXCONN)
#include <X11/Xtrans/Xtranssock.c>
#endif
#include <X11/Xtrans/Xtrans.c>
#include <X11/Xtrans/Xtransutil.c>
