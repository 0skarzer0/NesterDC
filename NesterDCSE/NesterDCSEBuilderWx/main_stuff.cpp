/* Created by Anjuta version 1.2.2 */
/*    This file will not be overwritten */

#include "main_stuff.h"
#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <string.h>
#include <dirent.h>
#include <math.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "nesdb.h"
#include <wx/wx.h>
#include <wx/zipstrm.h>
#include <wx/protocol/http.h>
#include <wx/filesys.h>
#include <wx/fs_inet.h>
#include <wx/wfstream.h>

bool copyfile(const char* fnSrc, const char* fnDest, const char* internalName)
{
    FILE* fpSrc = fopen(fnSrc, "rb");
    if (!fpSrc)
        return false;
    FILE* fpDest = fopen(fnDest, "wb");
    if (!fpDest)
    {
        fclose(fpSrc);
        return false;
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
    return true;
}

bool copyTree(const char* fnSrcDir, const char* fnDestDir)
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
            mkdir(sztempDest);
            copyTree(sztempSrc, sztempDest);
        }
        else
            copyfile(sztempSrc, sztempDest, NULL);
    }
    return true;
}

bool deleteTree(const char* fnSrcDir)
{
    char srcDir[0x1000];
    strcpy(srcDir, fnSrcDir);
    if (srcDir[strlen(srcDir)-1] != '/')
        strcat(srcDir, "/");
    DIR* dp = opendir(srcDir);
    struct dirent* dirinfo;
    char sztempSrc[0x1000];
    while (dirinfo = readdir(dp))
    {
        if (strcmp(dirinfo->d_name, ".") == 0 || strcmp(dirinfo->d_name, "..") == 0)
            continue;
        struct stat statbuf;
        sprintf(sztempSrc, "%s%s", srcDir, dirinfo->d_name);
        stat(sztempSrc, &statbuf);
        if (S_ISDIR(statbuf.st_mode))
        {
            deleteTree(sztempSrc);
            wxRmdir(sztempSrc);
        }
        else
            wxRemoveFile(sztempSrc);
    }
    return true;
}


bool copyfile2zip(const char* fnSrc, const char* fnDest, const char* internalName)
{
    wxFileSystem fs;
    wxFSFile* fsfp = fs.OpenFile(fnSrc);
    if (!fsfp)
        return false;
    wxInputStream* fis = fsfp->GetStream();
    char* buf = new char[fis->GetSize()];
    bool success = fis->Read(buf, fis->GetSize()).IsOk();
    if (!success)
    {
        delete[] buf;
        delete fsfp;
        return false;
    }
    
    wxFileOutputStream fos(fnDest);
    if (!fos.Ok())
        return false;
    wxZipOutputStream zos(fos);
    zos.PutNextEntry(internalName);
    zos.Write(buf, fis->GetSize());
    zos.Close();
    fos.Close();
    delete[] buf;
    delete fsfp;
    return true;
}

bool getFile(const char* szURL, const char* destFile)
{
    wxFileSystem fs;
    wxFSFile* fsfp = fs.OpenFile(szURL);
    if (!fsfp)
        return false;
    wxInputStream* fis =fsfp->GetStream();
    char* buf = new char[fis->GetSize()];
    bool success = fis->Read(buf, fis->GetSize()).IsOk();
    if (success)
    {
        FILE* fp = fopen(destFile, "wb");
        if (fp)
        {
            fwrite(buf, sizeof(char), fis->GetSize(), fp);
            fclose(fp);
        }
    }
    delete[] buf;
    delete fsfp;
    return success;
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
        temppow = (uint32) pow(16, digit);
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

int mymain(int argc, char** argv)
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
