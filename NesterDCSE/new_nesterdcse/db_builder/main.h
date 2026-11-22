#ifndef _MAIN_H_
#define _MAIN_H_

#if defined(__cplusplus) || defined(c_plusplus)

#define EXTERN_C extern "C"
#define START_EXTERN_C extern "C" {
#define END_EXTERN_C }

#else

#define EXTERN_C extern
#define START_EXTERN_C
#define END_EXTERN_C
	
#endif

#ifndef int32
#define int32	int
#endif

#ifndef uint32
#define uint32	unsigned int
#endif

#ifndef uint8
#define uint8	unsigned char
#endif

#ifndef bool
#define bool	unsigned char
#endif

#ifndef true
#define true	1
#endif

#ifndef false
#define false	0
#endif

uint8 hexstr_to_uint32(const char* str, uint32* retval);

#endif
