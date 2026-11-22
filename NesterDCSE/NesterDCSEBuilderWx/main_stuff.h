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
#define int32    int
#endif

#ifndef uint32
#define uint32    unsigned int
#endif

#ifndef uint8
#define uint8    unsigned char
#endif

bool copyTree(const char* fnSrcDir, const char* fnDestDir);
bool deleteTree(const char* fnSrcDir);
bool getFile(const char* szURL, const char* destFile);
bool copyfile(const char* fnSrc, const char* fnDest, const char* internalName);
bool copyfile2zip(const char* fnSrc, const char* fnDest, const char* internalName);
uint8 hexstr_to_uint32(const char* str, uint32* retval);

#endif
