#ifndef _PORT_KJS_H_
#define _PORT_KJS_H_

#include <kos.h>
#include <assert.h>
#include <errno.h>

#ifndef MAX
#  define MAX(a,b)  ((a) > (b)? (a) : (b))
#  define MIN(a,b)  ((a) < (b)? (a) : (b))
#endif

#define uint32_t uint32
#define int32_t int32

#endif
