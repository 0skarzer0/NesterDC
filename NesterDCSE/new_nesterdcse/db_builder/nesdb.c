#include "nesdb.h"
#include <string.h>
#include "crc32cool.h"
#include "unzip.h"

void NESDB_Init(struct NES_DB* pnesdb)
{
	pnesdb->fp = NULL;
	pnesdb->recCount = 0;
	pnesdb->index = NULL;
	pnesdb->filename[0] = '\0';
}

int NESDB_Open(struct NES_DB* pnesdb, const char* fn)
{
	pnesdb->fp = fopen(fn, "rb");
	if (!pnesdb->fp)
		return 0;
	strcpy(pnesdb->filename, fn);
	if (fread(&pnesdb->recCount, sizeof(unsigned int), 1, pnesdb->fp) == 0)
	{
		NESDB_Close(pnesdb);
		return 0;
	}
	if (pnesdb->recCount > 0)
	{
		pnesdb->index = (unsigned int*) malloc(sizeof(unsigned int)*pnesdb->recCount);
		if (fread(pnesdb->index, sizeof(unsigned int), pnesdb->recCount, pnesdb->fp) != pnesdb->recCount)
		{
			NESDB_Close(pnesdb);
			return 0;
		}
	}
	
	return 1;
}

int NESDB_Close(struct NES_DB* pnesdb)
{
	if (pnesdb->fp)
		fclose(pnesdb->fp);
	if (pnesdb->index)
		free(pnesdb->index);
	NESDB_Init(pnesdb);
	return 1;
}

int NESDB_Get(struct NES_DB* pnesdb, unsigned int index, struct NES_DB_ENTRY* data)
{
	fseek(pnesdb->fp, (sizeof(unsigned int)*(pnesdb->recCount+1)) + (sizeof(struct NES_DB_ENTRY)*index), SEEK_SET);
	fread(data, sizeof(struct NES_DB_ENTRY), 1, pnesdb->fp);
	return 1;
}

int NESDB_Search(struct NES_DB* pnesdb, unsigned int romcrc, struct NES_DB_ENTRY* data)
{
	unsigned int k = 0;
	for (; k < pnesdb->recCount; ++k)
	{
		if (pnesdb->index[k] == romcrc)
		{
			fseek(pnesdb->fp, (sizeof(unsigned int)*(pnesdb->recCount+1)) + (sizeof(struct NES_DB_ENTRY)*k), SEEK_SET);
			fread(data, sizeof(struct NES_DB_ENTRY), 1, pnesdb->fp);
			return 1;
		}
	}
	return 0;
}

int NESDB_New(struct NES_DB* pnesdb, const char* fn)
{
	FILE* fp = fopen(fn, "wb");
	unsigned int recCount = 0;
	fwrite(&recCount, sizeof(unsigned int), 1, fp);
	fclose(fp);
	return NESDB_Open(pnesdb, fn);
}

int NESDB_Add(struct NES_DB* pnesdb, struct NES_DB_ENTRY* data)
{
	char tempFilename[0x100];
	strcpy(tempFilename, pnesdb->filename);
	NESDB_Close(pnesdb);
	
	FILE* fp = fopen(tempFilename, "rb");
	fseek(fp, 0, SEEK_END);
	int filesize = ftell(fp);
	fseek(fp, 0, SEEK_SET);
	int newfilesize = filesize + sizeof(struct NES_DB_ENTRY) + sizeof(unsigned int);
	char* buf = (char*) malloc(newfilesize);
	fread(buf, sizeof(char), filesize, fp);
	fclose(fp);
	
	unsigned int* pRecCount = (unsigned int*) buf;
	
	char* indexbase = buf + sizeof(unsigned int);
	char* recordbase = indexbase + (sizeof(unsigned int)*(*pRecCount));
	
	if (*pRecCount > 0)
		memmove(recordbase+sizeof(unsigned int), recordbase, sizeof(struct NES_DB_ENTRY)*(*pRecCount));
	unsigned int* pNewCRC = (unsigned int*) recordbase;
	*pNewCRC = data->crc32;
	struct NES_DB_ENTRY* pNewEntry = (struct NES_DB_ENTRY*) ((buf + newfilesize) - sizeof(struct NES_DB_ENTRY));
	*pNewEntry = *data;
	*pRecCount += 1;
	
	fp = fopen(tempFilename, "wb");
	fwrite(buf, sizeof(char), newfilesize, fp);
	fclose(fp);
	
	free(buf);
	
	NESDB_Open(pnesdb, tempFilename);
	return 1;
}

int NESDB_CreateFromCSV(const char* csvfn, const char* dbfn)
{
	FILE* fpin = fopen(csvfn, "r");
	if (!fpin)
		return 0;
	
	FILE* fpout = fopen(dbfn, "wb");
	if (!fpout)
	{
		fclose(fpin);
		return 0;
	}
	
	char szcrc32[0x100];
	char szname[0x100];
	char sztitleshot[0x100];
	char szscreenshot[0x100];
	char szmanual[0x100];
	char szfaqs[0x100];
	char sznsf[0x100];
	char szboxscan[0x100];
	char szcartscan[0x100];
	char szstrategyguide[0x100];
	char szgamegenie[0x100];
	
	char* fields[11] = {szcrc32, szname, sztitleshot, szscreenshot, szmanual,
					 szfaqs, sznsf, szboxscan, szcartscan, szstrategyguide, szgamegenie};
	
	char szline[0x1000];
	
	unsigned int recordCount = 0;
	fgets(szline, 0x1000, fpin);
	while (fgets(szline, 0x1000, fpin))
		++recordCount;
	unsigned int k = 0;
	for (; k < (recordCount+1); ++k)
		fwrite(&recordCount, sizeof(unsigned int), 1, fpout);
	
	fseek(fpin, 0, SEEK_SET);
	fgets(szline, 0x1000, fpin);
	
	unsigned int curRecord = 0;
	while (fgets(szline, 0x1000, fpin))
	{
		//printf("%s", szline);
		
		unsigned int curField = 0;
		for (; curField < 11; ++curField)
			fields[curField][0] = '\0';
		
		unsigned int pos = 4;
		curField = 0;
		unsigned int lineLength = strlen(szline);
		for (; curField < 11; ++curField)
		{
			unsigned int fieldStart = pos;
			while (pos < lineLength)
			{
				if (szline[pos] == '\t' || szline[pos] == '\n')
				{
					unsigned int fieldLength = pos-fieldStart;
					strncpy(fields[curField], szline+fieldStart, fieldLength);
					fields[curField][fieldLength] = '\0';
					//printf("[%d]", fieldLength);
					//printf("%s\t", fields[curField]);
					pos++;
					break;
				}
				pos++;
			}
		}
		//printf("\n");
		//gets(szcrc32);
		
		struct NES_DB_ENTRY nesentry;
		
		nesentry.crc32 = 0;
		nesentry.titleshot = 0;
		nesentry.screenshot = 0;
		nesentry.manual = 0;
		nesentry.faqs = 0;
		nesentry.nsf = 0;
		nesentry.boxscan = 0;
		nesentry.cartscan = 0;
		nesentry.strategyguide = 0;
		nesentry.gamegenie = 0;
		nesentry.romnum = 0;
		nesentry.name[0] = '\0';
		
		sscanf(szcrc32, "%x", &(nesentry.crc32));
		sscanf(sztitleshot, "%d", &(nesentry.titleshot));
		sscanf(szscreenshot, "%d", &(nesentry.screenshot));
		sscanf(szmanual, "%d", &(nesentry.manual));
		sscanf(szfaqs, "%d", &(nesentry.faqs));
		sscanf(sznsf, "%d", &(nesentry.nsf));
		sscanf(szboxscan, "%d", &(nesentry.boxscan));
		sscanf(szcartscan, "%d", &(nesentry.cartscan));
		sscanf(szstrategyguide, "%d", &(nesentry.strategyguide));
		sscanf(szgamegenie, "%d", &(nesentry.gamegenie));
		strcpy(nesentry.name, szname);
		
		fseek(fpout, sizeof(unsigned int)*(curRecord+1), SEEK_SET);
		fwrite(&(nesentry.crc32), sizeof(unsigned int), 1, fpout);
		
		fseek(fpout, (sizeof(unsigned int)*(recordCount+1)) + (sizeof(struct NES_DB_ENTRY)*curRecord), SEEK_SET);
		fwrite(&nesentry, sizeof(struct NES_DB_ENTRY), 1, fpout);
		curRecord++;
	}
	
	fclose(fpin);
	fclose(fpout);
	return 1;
}

unsigned long CalcNESCRC32(const char* fn)
{
	FILE* fp = fopen(fn, "rb");
	if (!fp)
		return 0;
	fseek(fp, 0, SEEK_END);
	unsigned int filesize = ftell(fp);
	if (filesize == 0)
	{
		fclose(fp);
		return 0;
	}
	fseek(fp, 16, SEEK_SET);
	filesize -= 16;
	unsigned char* romdata = (unsigned char*) malloc(filesize);
	fread(romdata, sizeof(unsigned char), filesize, fp);
	fclose(fp);
	unsigned long crc;
	crc32Init(&crc);
	crc32Update(&crc, romdata, filesize);
	crc32Finish(&crc);
	free(romdata);
	return crc;
}

unsigned long CalcNESCRC32Zip(const char* fn)
{
	unsigned long crc;
	char name[132];
	char filename[132];
	int port;
	unzFile file;
	unz_file_info info;
	int filesize = 0;
	int len;

	file = unzOpen(fn);
	if (!file)
		return 0;
	port = unzGoToFirstFile(file);
	while (port == UNZ_OK)
	{
		unzGetCurrentFileInfo(file, &info, name, 128, NULL, 0, NULL, 0);
		len = strlen(name);
		if (info.uncompressed_size > filesize && strlen(name) > 4 && stricmp(name + (strlen(name)-4), ".nes") == 0)
		{
			strcpy(filename, name);
			filesize = info.uncompressed_size;
		}
		port = unzGoToNextFile(file);
	}
	if (filesize == 0)
	{
		unzClose(file);
		return 0;
	}
	unzLocateFile(file, filename, 1);
	unzGetCurrentFileInfo(file, &info, filename, 128, NULL, 0, NULL, 0);
	if (unzOpenCurrentFile(file) != UNZ_OK)
	{
		unzClose(file);
		return 0;
	}
	unsigned char* uctemp = (unsigned char*) malloc(filesize);
	unzReadCurrentFile(file, uctemp, filesize);
	if(unzCloseCurrentFile(file) == UNZ_CRCERROR)
	{
		unzClose(file);
		return 0;
	}
	crc32Init(&crc);
	crc32Update(&crc, uctemp + 16, filesize - 16);
	crc32Finish(&crc);
	unzClose(file);
	free(uctemp);
	return crc;
}
