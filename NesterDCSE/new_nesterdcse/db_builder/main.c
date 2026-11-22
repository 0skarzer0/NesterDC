/* Created by Anjuta version 1.2.2 */
/*	This file will not be overwritten */

#include "main.h"
#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <string.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "nesdb.h"
#include "zip.h"
#include <ghttp.h>
#include "XML.h"

typedef struct SSettings
{
	char roms_path[0x1000];
	char dest_path[0x1000];
	char library_url[0x1000];
	char stage_path[0x1000];
	bool get_carts;
	bool get_manuals;
	bool get_faqs;
	bool get_titles;
	bool get_screenshots;
	bool get_boxes;
	bool get_nsfs;
	bool get_gamegenie;
} Settings;

Settings* LoadBuildCfg(const char* szcfgpath)
{
	XML* rootxml = XML_OpenFile("buildcfg.xml");
	if (!rootxml)
		return NULL;
	Settings* s = malloc(sizeof(Settings));
	memset(s, 0, sizeof(Settings));
	s->get_carts = true;
	s->get_manuals = true;
	s->get_faqs = true;
	s->get_titles = true;
	s->get_screenshots = true;
	s->get_boxes = true;
	s->get_nsfs = true;
	s->get_gamegenie = true;
	uint32 k = 0;
	for (; k < rootxml->attribute_names.count; ++k)
	{
		const char* szname = rootxml->attribute_names.data[k];
		const char* szvalue = rootxml->attribute_values.data[k];
		if (strcmp(szname, "get_carts") == 0)
			s->get_carts = strcmp(szvalue, "true") == 0;
		else if (strcmp(szname, "get_manuals") == 0)
			s->get_manuals = strcmp(szvalue, "true") == 0;
		else if (strcmp(szname, "get_faqs") == 0)
			s->get_faqs = strcmp(szvalue, "true") == 0;
		else if (strcmp(szname, "get_titles") == 0)
			s->get_titles = strcmp(szvalue, "true") == 0;
		else if (strcmp(szname, "get_screenshots") == 0)
			s->get_screenshots = strcmp(szvalue, "true") == 0;
		else if (strcmp(szname, "get_boxes") == 0)
			s->get_boxes = strcmp(szvalue, "true") == 0;
		else if (strcmp(szname, "get_nsfs") == 0)
			s->get_nsfs = strcmp(szvalue, "true") == 0;
		else if (strcmp(szname, "get_gamegenie") == 0)
			s->get_gamegenie = strcmp(szvalue, "true") == 0;
		else if (strcmp(szname, "get_manuals") == 0)
			s->get_manuals = strcmp(szvalue, "true") == 0;
		else if (strcmp(szname, "roms_path") == 0)
			strcpy(s->roms_path, szvalue);
		else if (strcmp(szname, "dest_path") == 0)
			strcpy(s->dest_path, szvalue);
		else if (strcmp(szname, "library_url") == 0)
			strcpy(s->library_url, szvalue);
		else if (strcmp(szname, "stage_path") == 0)
			strcpy(s->stage_path, szvalue);
	}
	return s;
}

uint8 hexstr_to_uint32(const char* str, uint32* retval)
{
	uint32 num = 0;
	int32 len = strlen(str);
	if (len == 0)
		return 0;
	uint32 digit = 0;
	uint32 temppow;
	int32 k;
	for (k = len - 1; k >= 0; --k)
	{
		temppow = pow(16, digit);
		switch (str[k])
		{
		case '0':
			break;
		case '1':
			num += temppow;
			break;
		case '2':
			num += temppow * 2;
			break;
		case '3':
			num += temppow * 3;
			break;
		case '4':
			num += temppow * 4;
			break;
		case '5':
			num += temppow * 5;
			break;
		case '6':
			num += temppow * 6;
			break;
		case '7':
			num += temppow * 7;
			break;
		case '8':
			num += temppow * 8;
			break;
		case '9':
			num += temppow * 9;
			break;
		case 'a':
		case 'A':
			num += temppow * 10;
			break;
		case 'b':
		case 'B':
			num += temppow * 11;
			break;
		case 'c':
		case 'C':
			num += temppow * 12;
			break;
		case 'd':
		case 'D':
			num += temppow * 13;
			break;
		case 'e':
		case 'E':
			num += temppow * 14;
			break;
		case 'f':
		case 'F':
			num += temppow * 15;
			break;
		default:
		{
			printf("hexstr_to_uint32() bad char: %c\n", str[k]);
			return 0;
		}
		};
		++digit;
	}
	*retval = num;
	return 1;
}

char copyfile(const char* fnSrc, const char* fnDest, const char* internalName)
{
	FILE* fpSrc = fopen(fnSrc, "rb");
	if (!fpSrc)
		return 0;
	FILE* fpDest = fopen(fnDest, "wb");
	if (!fpDest)
	{
		fclose(fpSrc);
		return 0;
	}
	char buf[0x1000];
	int itemp;
	while (itemp = fread(buf, sizeof(char), 0x1000, fpSrc))
	{
		fwrite(buf, sizeof(char), itemp, fpDest);
		if (itemp != 0x1000)
			break;
	}
	fclose(fpSrc);
	fclose(fpDest);
	return 1;
}

char copyTree(const char* fnSrcDir, const char* fnDestDir)
{
	char srcDir[0x1000];
	char destDir[0x1000];
	strcpy(srcDir, fnSrcDir);
	strcpy(destDir, fnDestDir);
	if (srcDir[strlen(srcDir)-1] != '/')
		strcat(srcDir, "/");
	if (destDir[strlen(destDir)-1] != '/')
		strcat(destDir, "/");
	DIR* dp = opendir(srcDir);
	struct dirent* dirinfo;
	char sztempSrc[0x1000];
	char sztempDest[0x1000];
	while (dirinfo = readdir(dp))
	{
		if (strcmp(dirinfo->d_name, ".") == 0 || strcmp(dirinfo->d_name, "..") == 0)
			continue;
		struct stat statbuf;
		sprintf(sztempSrc, "%s%s", srcDir, dirinfo->d_name);
		sprintf(sztempDest, "%s%s", destDir, dirinfo->d_name);
		stat(sztempSrc, &statbuf);
		if (S_ISDIR(statbuf.st_mode))
		{
			mkdir(sztempDest, S_IRWXU|S_IRWXG|S_IRWXO);
			copyTree(sztempSrc, sztempDest);
		}
		else
			copyfile(sztempSrc, sztempDest, NULL);
	}
	return 1;
}

char copyfile2zip(const char* fnSrc, const char* fnDest, const char* internalName)
{
	FILE* fpSrc = fopen(fnSrc, "rb");
	if (!fpSrc)
		return 0;
	zipFile fpDest = zipOpen(fnDest, 0);
	if (!fpDest)
	{
		fclose(fpSrc);
		return 0;
	}
	int zi = zipOpenNewFileInZip(fpDest,
                       internalName,
                       NULL,
                       NULL,
                       0,
                       NULL,
                       0,
                       NULL,
                       Z_DEFLATED,
                       Z_DEFAULT_COMPRESSION);
	char buf[0x1000];
	int itemp;
	while (itemp = fread(buf, sizeof(char), 0x1000, fpSrc))
	{
		zipWriteInFileInZip(fpDest, buf, itemp);
		if (itemp != 0x1000)
			break;
	}
	fclose(fpSrc);
	zipCloseFileInZip(fpDest);
	zipClose(fpDest, NULL);
	return 1;
}

int getHTTPFile(const char* szURL, const char* destFile)
{
	char locURL[0x1000];
	strcpy(locURL, szURL);
	ghttp_request *request = NULL;
	request = ghttp_request_new();
	ghttp_set_uri(request, locURL);
	ghttp_set_header(request, http_hdr_Connection, "close");
	ghttp_set_sync(request, ghttp_sync);
	ghttp_set_type(request, ghttp_type_get);
	ghttp_prepare(request);
	ghttp_process(request);
	
	int code = ghttp_status_code(request);
	
	if (code == 200)
	{
		FILE* fp = fopen(destFile, "wb");
		if (!fp)
		{
			ghttp_request_destroy(request);
			return 0;
		}	
		fwrite(ghttp_get_body(request), 1, ghttp_get_body_len(request), fp);
		fclose(fp);
	}
	ghttp_request_destroy(request);
	return (code == 200);
}

int getFile(const char* szURL, const char* destFile)
{
	static const char* szHTTPprotocol = "http://";
	if (strncmp(szURL, szHTTPprotocol, strlen(szHTTPprotocol)) == 0)
		return getHTTPFile(szURL, destFile);
	else
	{
		FILE* fpin = fopen(szURL, "rb");
		if (!fpin)
			return 0;
		FILE* fpout = fopen(destFile, "wb");
		if (!fpout)
		{
			fclose(fpin);
			return 0;
		}
		char buf[0x1000];
		while (!feof(fpin))
		{
			size_t b = fread(buf, sizeof(char), 0x1000, fpin);
			fwrite(buf, sizeof(char), b, fpout);
		}
		fclose(fpin);
		fclose(fpout);
		return 1;
	}
}

int main(int argc, char** argv)
{
	printf("\n");
	if (argc < 2)
	{
		printf("tell me what to do\n");
		return 0;
	}
	
	const char* action = argv[1];
	if (stricmp(action, "tab") == 0)
	{
		if (argc < 4)
		{
			printf("i need input and output files\n");
			return 0;
		}
		printf("Creating NES database from tab-delimited text file...\n");
		NESDB_CreateFromCSV(argv[2], argv[3]);
		printf("DONE!\n");
		return 0;
	}
	
	if (stricmp(action, "build") == 0)
	{
		if (argc < 3)
		{
			printf("build action syntax:\n\n");
			printf("nesterdcsebuild build buildcfg.xml\n\n");
			return 1;
		}
		
		Settings* s = LoadBuildCfg(argv[2]);
		if (!s)
		{
			printf("Couldn't load config file: %s\n\n", argv[2]);
			return 1;
		}
		
		if (s->roms_path[strlen(s->roms_path)-1] != '/')
			strcat(s->roms_path, "/");
		if (s->roms_path[strlen(s->dest_path)-1] != '/')
			strcat(s->dest_path, "/");
		if (s->roms_path[strlen(s->library_url)-1] != '/')
			strcat(s->library_url, "/");
		if (s->roms_path[strlen(s->stage_path)-1] != '/')
			strcat(s->stage_path, "/");
		
		char szMasterNESDBURL[0x1000];
		sprintf(szMasterNESDBURL, "%smaster_nes.db", s->library_url);
		getFile(szMasterNESDBURL, "master_nes.db");
		
		struct NES_DB nesdb;
		NESDB_Init(&nesdb);
		NESDB_Open(&nesdb, "master_nes.db");

		struct NES_DB_ENTRY dbentry;
		
		mkdir(s->dest_path, S_IRWXU|S_IRWXG|S_IRWXO);
		
		copyTree(s->stage_path, s->dest_path);
		
		char szLibraryPath[0x1000];
		sprintf(szLibraryPath, "%slibrary/", s->dest_path);
		mkdir(szLibraryPath, S_IRWXU|S_IRWXG|S_IRWXO);
		
		char szDestROMPath[0x1000];
		sprintf(szDestROMPath, "%sroms/", szLibraryPath);
		mkdir(szDestROMPath, S_IRWXU|S_IRWXG|S_IRWXO);
		
		char szDestCartScansPath[0x1000];
		sprintf(szDestCartScansPath, "%sCartScans/", szLibraryPath);
		mkdir(szDestCartScansPath, S_IRWXU|S_IRWXG|S_IRWXO);
		
		char szDestManualsPath[0x1000];
		sprintf(szDestManualsPath, "%sManuals/", szLibraryPath);
		mkdir(szDestManualsPath, S_IRWXU|S_IRWXG|S_IRWXO);
		
		char szDestFAQPath[0x1000];
		sprintf(szDestFAQPath, "%sFAQs/", szLibraryPath);
		mkdir(szDestFAQPath, S_IRWXU|S_IRWXG|S_IRWXO);
		
		char szDestTitleShotsPath[0x1000];
		sprintf(szDestTitleShotsPath, "%sTitleShots/", szLibraryPath);
		mkdir(szDestTitleShotsPath, S_IRWXU|S_IRWXG|S_IRWXO);
		
		char szDestScreenShotsPath[0x1000];
		sprintf(szDestScreenShotsPath, "%sScreenShots/", szLibraryPath);
		mkdir(szDestScreenShotsPath, S_IRWXU|S_IRWXG|S_IRWXO);
		
		char szDestBoxScansPath[0x1000];
		sprintf(szDestBoxScansPath, "%sBoxScans/", szLibraryPath);
		mkdir(szDestBoxScansPath, S_IRWXU|S_IRWXG|S_IRWXO);
		
		char szDestNSFPath[0x1000];
		sprintf(szDestNSFPath, "%snsf/", szLibraryPath);
		mkdir(szDestNSFPath, S_IRWXU|S_IRWXG|S_IRWXO);
		
		char szDestGameGeniePath[0x1000];
		sprintf(szDestGameGeniePath, "%sGameGenie/", szLibraryPath);
		mkdir(szDestGameGeniePath, S_IRWXU|S_IRWXG|S_IRWXO);
		
		char szOutputDB[0x1000];
		sprintf(szOutputDB, "%sram/mynes.db", s->dest_path);
		
		struct NES_DB myNesDB;
		NESDB_Init(&myNesDB);
		NESDB_New(&myNesDB, szOutputDB);
		
		DIR* dp = opendir(s->roms_path);
		struct dirent* dirinfo;
		char sztemp[0x1000];
		unsigned int curROMnum = 1;
		while (dirinfo = readdir(dp))
		{
			if (strcmp(dirinfo->d_name, ".") == 0 || strcmp(dirinfo->d_name, "..") == 0)
				continue;
			struct stat statbuf;
			sprintf(sztemp, "%s%s", s->roms_path, dirinfo->d_name);
			stat(sztemp, &statbuf);
			if (S_ISDIR(statbuf.st_mode))
				continue;
			unsigned long crc;
			char isZIP;
			if (stricmp(sztemp + (strlen(sztemp)-4), ".zip") == 0)
			{
				crc = CalcNESCRC32Zip(sztemp);
				isZIP = 1;
			}
			else if (stricmp(sztemp + (strlen(sztemp)-4), ".nes") == 0)
			{
				crc = CalcNESCRC32(sztemp);
				isZIP = 0;
			}
			else
				continue;
			if (crc)
			{
				if (NESDB_Search(&nesdb, crc, &dbentry))
				{
					printf("%s\n", dbentry.name);
					fflush(stdout);
					
					dbentry.romnum = curROMnum;
					char destemp[0x1000];
					
					char (*CopyFunc)(const char*, const char*, const char*);
					CopyFunc = NULL;
					
					sprintf(destemp, "%s%04d.zip", szDestROMPath, curROMnum);
					if (isZIP)
						CopyFunc = copyfile;
					else
						CopyFunc = copyfile2zip;
					
					char internalName[0x100];
					sprintf(internalName, "%s.nes", dbentry.name);
					if (!CopyFunc(sztemp, destemp, internalName))
					{
						printf("    error: couldn't copy rom!\n");
						fflush(stdout);
						continue;
					}
					
					if (dbentry.cartscan)
					{
						if (s->get_carts)
						{
							char szTempURL[0x1000];
							sprintf(szTempURL, "%sCartScans/%04d.jpg", s->library_url, dbentry.cartscan);
							char szTempDest[0x1000];
							sprintf(szTempDest, "%s%04d.jpg", szDestCartScansPath, dbentry.cartscan);
							printf("    Get Cart Scan: ");
							fflush(stdout);
							if (getFile(szTempURL, szTempDest))
							{
								printf("OK\n");
								fflush(stdout);
							}
							else
							{
								dbentry.cartscan = 0;
								printf("Failed\n");
								fflush(stdout);
							}
						}
						else
							dbentry.cartscan = 0;
					}
					
					if (dbentry.faqs)
					{
						if (s->get_faqs && 0)
						{
							char szTempURL[0x1000];
							sprintf(szTempURL, "%sFAQs/%04d.zip", s->library_url, dbentry.faqs);
							char szTempDest[0x1000];
							sprintf(szTempDest, "%s%04d.zip", szDestFAQPath, dbentry.faqs);
							printf("    Get FAQs: ");
							fflush(stdout);
							if (getFile(szTempURL, szTempDest))
							{
								printf("OK\n");
								fflush(stdout);
							}
							else
							{
								dbentry.faqs = 0;
								printf("Failed\n");
								fflush(stdout);
							}
						}
						else
							dbentry.faqs = 0;
					}
					
					if (dbentry.titleshot)
					{
						if (s->get_titles)
						{
							char szTempURL[0x1000];
							sprintf(szTempURL, "%sTitleShots/%04d.png", s->library_url, dbentry.titleshot);
							char szTempDest[0x1000];
							sprintf(szTempDest, "%s%04d.png", szDestTitleShotsPath, dbentry.titleshot);
							printf("    Get Title Shot: ");
							fflush(stdout);
							if (getFile(szTempURL, szTempDest))
							{
								printf("OK\n");
								fflush(stdout);
							}
							else
							{
								dbentry.titleshot = 0;
								printf("Failed\n");
								fflush(stdout);
							}
						}
						else
							dbentry.titleshot = 0;
					}
					
					if (dbentry.screenshot)
					{
						if (s->get_screenshots)
						{
							char szTempURL[0x1000];
							sprintf(szTempURL, "%sScreenShots/%04d.png", s->library_url, dbentry.screenshot);
							char szTempDest[0x1000];
							sprintf(szTempDest, "%s%04d.png", szDestScreenShotsPath, dbentry.screenshot);
							printf("    Get Screen Shot: ");
							fflush(stdout);
							if (getFile(szTempURL, szTempDest))
							{
								printf("OK\n");
								fflush(stdout);
							}
							else
							{
								dbentry.screenshot = 0;
								printf("Failed\n");
								fflush(stdout);
							}
						}
						else
							dbentry.screenshot = 0;
					}
					
					if (dbentry.manual)
					{
						if (s->get_manuals)
						{
							char tempdir[0x1000];
							sprintf(tempdir, "%s%04d/", szDestManualsPath, dbentry.manual);
							mkdir(tempdir, S_IRWXU|S_IRWXG|S_IRWXO);
							printf("    Get Manual: ");
							fflush(stdout);

							int page = 0;
							while (1)
							{
								char szTempURL[0x1000];
								sprintf(szTempURL, "%sManuals/%04d/%02d.jpg", s->library_url, dbentry.manual, page);
								char szTempDest[0x1000];
								sprintf(szTempDest, "%s%02d.jpg", tempdir, page);
								if (!getFile(szTempURL, szTempDest))
								{
									if (page == 0)
									{
										dbentry.manual = 0;
										printf("Falied\n", sztemp);
										fflush(stdout);
									}
									break;
								}
								page++;
							}
							if (page > 0)
							{
								printf("OK\n");
								fflush(stdout);
							}
						}
						else
							dbentry.manual = 0;
					}
					
					if (dbentry.nsf)
					{
						if (s->get_nsfs)
						{
							char szTempURL[0x1000];
							sprintf(szTempURL, "%snsf/%04d.zip", s->library_url, dbentry.nsf);
							char szTempDest[0x1000];
							sprintf(szTempDest, "%s%04d.zip", szDestNSFPath, dbentry.nsf);
							printf("    Get NSF: ");
							fflush(stdout);
							//printf(" %s ", szTempURL);
							if (getFile(szTempURL, szTempDest))
							{
								printf("OK\n");
								fflush(stdout);
							}
							else
							{
								dbentry.nsf = 0;
								printf("Failed\n");
								fflush(stdout);
							}
						}
						else
							dbentry.nsf = 0;
					}
					
					if (dbentry.boxscan)
					{
						if (s->get_boxes)
						{
							char szTempURL[0x1000];
							sprintf(szTempURL, "%sBoxScans/%04d.jpg", s->library_url, dbentry.boxscan);
							char szTempDest[0x1000];
							sprintf(szTempDest, "%s%04d.jpg", szDestBoxScansPath, dbentry.boxscan);
							printf("    Get Box Scan: ");
							fflush(stdout);
							//printf(" %s ", szTempURL);
							if (getFile(szTempURL, szTempDest))
							{
								printf("OK\n");
								fflush(stdout);
							}
							else
							{
								dbentry.boxscan = 0;
								printf("Failed\n");
								fflush(stdout);
							}
						}
						else
							dbentry.boxscan = 0;
					}
					
					if (dbentry.gamegenie)
					{
						if (s->get_gamegenie)
						{
							char szTempURL[0x1000];
							sprintf(szTempURL, "%sGameGenie/%04d.xml", s->library_url, dbentry.gamegenie);
							char szTempDest[0x1000];
							sprintf(szTempDest, "%s%04d.xml", szDestGameGeniePath, dbentry.gamegenie);
							printf("    Get Game Genie Codes: ");
							fflush(stdout);
							//printf(" %s ", szTempURL);
							if (getFile(szTempURL, szTempDest))
							{
								printf("OK\n");
								fflush(stdout);
							}
							else
							{
								dbentry.gamegenie = 0;
								printf("Failed\n");
								fflush(stdout);
							}
						}
						else
							dbentry.gamegenie = 0;
					}
					
					NESDB_Add(&myNesDB, &dbentry);
					
					curROMnum++;
				}
				else
					printf("\nerror: %s: not found in db\n", sztemp);
			}
			else
				printf("\nerror: %s: file not found\n", sztemp);
		}
		closedir(dp);
		NESDB_Close(&nesdb);
		NESDB_Close(&myNesDB);
		return 0;
	}
	
	if (stricmp(action, "test") == 0)
	{
		mkdir("test2", S_IRWXU|S_IRWXG|S_IRWXO);
		copyTree("test", "test2");
		printf("DONE\n\n");
		return 0;
	}
	
	if (stricmp(action, "dump") == 0)
	{
		if (argc < 3)
		{
			printf("what should i dump?\n");
			return 0;
		}
		
		struct NES_DB nesdb;
		NESDB_Init(&nesdb);
		NESDB_Open(&nesdb, argv[2]);
		
		unsigned int k = 0;
		for (; k < nesdb.recCount; ++k)
		{
			struct NES_DB_ENTRY dbentry;
			NESDB_Get(&nesdb, k, &dbentry);
			printf("%s\n", dbentry.name);
		}
		
		return 0;
	}
	
	printf("I don't know that action!\n");
	
	return 0;
}

/*
struct NES_DB nesdb;
NESDB_Init(&nesdb);
NESDB_Open(&nesdb, "nes.db");

struct NES_DB myNesDB;
NESDB_Init(&myNesDB);
NESDB_New(&myNesDB, "mynes.db");

struct NES_DB_ENTRY dbentry;

NESDB_Search(&nesdb, 0x9747ac09, &dbentry);
NESDB_Add(&myNesDB, &dbentry);

NESDB_Search(&nesdb, 0xac25e1f4, &dbentry);
NESDB_Add(&myNesDB, &dbentry);

NESDB_Search(&nesdb, 0x90d3210a, &dbentry);
NESDB_Add(&myNesDB, &dbentry);

NESDB_Search(&nesdb, 0x32fb0583, &dbentry);
NESDB_Add(&myNesDB, &dbentry);

NESDB_Search(&nesdb, 0xa8f5c2ab, &dbentry);
NESDB_Add(&myNesDB, &dbentry);

NESDB_Search(&myNesDB, 0x90d3210a, &dbentry);
printf("%s\n\n", dbentry.name);

NESDB_Close(&nesdb);
NESDB_Close(&myNesDB);
*/
