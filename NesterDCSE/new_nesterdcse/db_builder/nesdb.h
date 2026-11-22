#ifndef _NESDB_H
#define _NESDB_H

#include <stdlib.h>
#include <stdio.h>

#ifdef __cplusplus
extern "C"
{
#endif

struct NES_DB
{
	FILE* fp;
	unsigned int recCount;
	unsigned int* index;
	char filename[0x100];
};

struct NES_DB_ENTRY
{
	unsigned int crc32;
	unsigned int titleshot;
	unsigned int screenshot;
	unsigned int manual;
	unsigned int faqs;
	unsigned int nsf;
	unsigned int boxscan;
	unsigned int cartscan;
	unsigned int strategyguide;
	unsigned int gamegenie;
	unsigned int romnum; // NOT a field in the TAB
	char name[0x80];
};

void NESDB_Init(struct NES_DB* pnesdb);
int NESDB_Open(struct NES_DB* pnesdb, const char* fn);
int NESDB_Close(struct NES_DB* pnesdb);
int NESDB_Get(struct NES_DB* pnesdb, unsigned int index, struct NES_DB_ENTRY* data);
int NESDB_Search(struct NES_DB* pnesdb, unsigned int romcrc, struct NES_DB_ENTRY* data);

int NESDB_New(struct NES_DB* pnesdb, const char* fn);
int NESDB_Add(struct NES_DB* pnesdb, struct NES_DB_ENTRY* data);

int NESDB_CreateFromCSV(const char* csvfn, const char* dbfn);

unsigned long CalcNESCRC32(const char* fn);
unsigned long CalcNESCRC32Zip(const char* fn);

#ifdef __cplusplus
}
#endif

#endif /* _NESDB_H */
