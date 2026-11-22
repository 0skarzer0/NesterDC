#include <kos.h>
#include "dc_utils.h"

START_EXTERN_C

void _splitpath(const char *path, char *drive, char *dir, char *fname, char *ext);
void _makepath(char *path, const char * something, const char *dir, const char *fname, const char *ext);

END_EXTERN_C
