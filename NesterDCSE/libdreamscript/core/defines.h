#ifndef DEFINES_H
#define DEFINES_H

#define HAVE_ENCDEC 0
#define HAVE_STRDUP 1
#define HAVE_STRNLEN 1
#define HAVE_EXPAT 0
#define HAVE_MBSTATE 1
#define HAVE_WCWIDTH 1
#define HAVE_SNPRINTF 1
#define HAVE_VSNPRINTF 0
#define HAVE_VARMACRO 1
#define HAVE_LANGINFO 1

#ifdef __GNUC__
#define UNUSED __attribute__ ((unused))
#else
#define UNUSED
#endif

#endif /* DEFINES_H */
