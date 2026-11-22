#include "cdbuildthread.h"
#include "XML.h"
#include "main_stuff.h"
#include "nesdb.h"
#include <dirent.h>
#include "MyDlg.h"
#include <wx/process.h>

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
    uint32 image_format;
} Settings;

Settings* LoadBuildCfg(const char* szcfgpath)
{
    XML* rootxml = XML_OpenFile("buildcfg.xml");
    if (!rootxml)
        return NULL;
    Settings* s = (Settings*) malloc(sizeof(Settings));
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
        else if (strcmp(szname, "image_format") == 0)
            s->image_format = (strcmp(szvalue, "0") == 0) ? 0 : 1;
    }
    return s;
}

CDBuildThread::CDBuildThread(const char* cfgFile, wxTextCtrl* txtOut, MyDlg* pDlg)
    : wxThread(wxTHREAD_JOINABLE)
{
    configFile = cfgFile;
    txtOutput = txtOut;
    dlg = pDlg;
}
    
wxThread::ExitCode CDBuildThread::Entry()
{
    Settings* s = LoadBuildCfg(configFile);
    if (!s)
    {
        txtOutput->AppendText("Couldn't load config xml file\n");
        return 0;
    }
    
    if (s->roms_path[strlen(s->roms_path)-1] != '/' || s->roms_path[strlen(s->roms_path)-1] != '\\')
        strcat(s->roms_path, "/");
    if (s->dest_path[strlen(s->dest_path)-1] != '/' || s->dest_path[strlen(s->dest_path)-1] != '\\')
        strcat(s->dest_path, "/");
    if (s->library_url[strlen(s->library_url)-1] != '/' || s->library_url[strlen(s->library_url)-1] != '\\')
        strcat(s->library_url, "/");
    if (s->stage_path[strlen(s->stage_path)-1] != '/' || s->stage_path[strlen(s->stage_path)-1] != '\\')
        strcat(s->stage_path, "/");
    
    char szMasterNESDBURL[0x1000];
    sprintf(szMasterNESDBURL, "%smaster_nes.db", s->library_url);
    txtOutput->AppendText("Downloading ");
    txtOutput->AppendText(szMasterNESDBURL);
    txtOutput->AppendText("... ");
    
    if (!getFile(szMasterNESDBURL, "master_nes.db"))
    {
        txtOutput->AppendText("Failed\n");
        return 0;
    }
    txtOutput->AppendText(" OK!\n");
    if (TestDestroy())
    {
        delete s;
        return 0;
    }
    
    struct NES_DB nesdb;
    NESDB_Init(&nesdb);
    NESDB_Open(&nesdb, "master_nes.db");

    struct NES_DB_ENTRY dbentry;
    
    mkdir(s->dest_path);
    
    txtOutput->AppendText("Copying pre_cd to destination... ");
    copyTree(s->stage_path, s->dest_path);
    if (TestDestroy())
    {
        delete s;
        return 0;
    }
    txtOutput->AppendText("OK!\n");
    
    char szLibraryPath[0x1000];
    sprintf(szLibraryPath, "%slibrary/", s->dest_path);
    bool hasLocalCache = false;
    if (wxDirExists(szLibraryPath))
    {
        txtOutput->AppendText("Backing up previous library locally... ");
        if (wxDirExists("local_library"))
        {
            wxString oldRomsFolder = szLibraryPath;
            oldRomsFolder += "roms/";
            deleteTree(oldRomsFolder);
            copyTree(szLibraryPath, "local_library");
            deleteTree(szLibraryPath);
            wxRmdir(szLibraryPath);
        }
        else
            wxRenameFile(szLibraryPath, "local_library");
        hasLocalCache = true;
        txtOutput->AppendText("OK!\n");
    }
    
    txtOutput->AppendText("creating library folder... ");
    mkdir(szLibraryPath);
    txtOutput->AppendText("OK!\n");
    
    txtOutput->AppendText("creating roms folder... ");
    char szDestROMPath[0x1000];
    sprintf(szDestROMPath, "%sroms/", szLibraryPath);
    mkdir(szDestROMPath);
    txtOutput->AppendText("OK!\n");
    
    txtOutput->AppendText("creating library folder... ");
    char szDestCartScansPath[0x1000];
    sprintf(szDestCartScansPath, "%sCartScans/", szLibraryPath);
    mkdir(szDestCartScansPath);
    txtOutput->AppendText("OK!\n");
    
    txtOutput->AppendText("creating Manuals folder... ");
    char szDestManualsPath[0x1000];
    sprintf(szDestManualsPath, "%sManuals/", szLibraryPath);
    mkdir(szDestManualsPath);
    txtOutput->AppendText("OK!\n");
    
    txtOutput->AppendText("creating FAQs folder... ");
    char szDestFAQPath[0x1000];
    sprintf(szDestFAQPath, "%sFAQs/", szLibraryPath);
    mkdir(szDestFAQPath);
    txtOutput->AppendText("OK!\n");
    
    txtOutput->AppendText("creating TitleShots folder... ");
    char szDestTitleShotsPath[0x1000];
    sprintf(szDestTitleShotsPath, "%sTitleShots/", szLibraryPath);
    mkdir(szDestTitleShotsPath);
    txtOutput->AppendText("OK!\n");
    
    txtOutput->AppendText("creating ScreenShots folder... ");
    char szDestScreenShotsPath[0x1000];
    sprintf(szDestScreenShotsPath, "%sScreenShots/", szLibraryPath);
    mkdir(szDestScreenShotsPath);
    txtOutput->AppendText("OK!\n");
    
    txtOutput->AppendText("creating BoxScans folder... ");
    char szDestBoxScansPath[0x1000];
    sprintf(szDestBoxScansPath, "%sBoxScans/", szLibraryPath);
    mkdir(szDestBoxScansPath);
    txtOutput->AppendText("OK!\n");
    
    txtOutput->AppendText("creating nsf folder... ");
    char szDestNSFPath[0x1000];
    sprintf(szDestNSFPath, "%snsf/", szLibraryPath);
    mkdir(szDestNSFPath);
    txtOutput->AppendText("OK!\n");
    
    txtOutput->AppendText("creating GameGenie folder... ");
    char szDestGameGeniePath[0x1000];
    sprintf(szDestGameGeniePath, "%sGameGenie/", szLibraryPath);
    mkdir(szDestGameGeniePath);
    txtOutput->AppendText("OK!\n");
    
    txtOutput->AppendText("opening mynes.db for writing... ");
    char szOutputDB[0x1000];
    sprintf(szOutputDB, "%sram/mynes.db", s->dest_path);
    txtOutput->AppendText("OK!\n");
    
    struct NES_DB myNesDB;
    NESDB_Init(&myNesDB);
    NESDB_New(&myNesDB, szOutputDB);
    
    DIR* dp = opendir(s->roms_path);
    struct dirent* dirinfo;
    char sztemp[0x1000];
    unsigned int curROMnum = 1;
    while (dirinfo = readdir(dp))
    {
        if (TestDestroy())
            break;
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
                txtOutput->AppendText(dbentry.name);
                txtOutput->AppendText("\n");
                
                dbentry.romnum = curROMnum;
                char destemp[0x1000];
                
                bool (*CopyFunc)(const char*, const char*, const char*);
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
                    txtOutput->AppendText("    error: couldn't copy rom!\n");
                    continue;
                }
                
                if (dbentry.cartscan)
                {
                    if (s->get_carts)
                    {
                        txtOutput->AppendText("    Get Cart Scan: ");
                        char szTempURL[0x1000];
                        char szTempDest[0x1000];
                        sprintf(szTempDest, "%s%04d.jpg", szDestCartScansPath, dbentry.cartscan);
                        bool gotit = false;
                        if (hasLocalCache)
                        {
                            sprintf(szTempURL, "local_library/CartScans/%04d.jpg", dbentry.cartscan);
                            gotit = getFile(szTempURL, szTempDest);
                        }
                        if (!gotit)
                        {
                            sprintf(szTempURL, "%sCartScans/%04d.jpg", s->library_url, dbentry.cartscan);
                            gotit = getFile(szTempURL, szTempDest);
                        }
                        if (gotit)
                            txtOutput->AppendText("OK\n");
                        else
                        {
                            dbentry.cartscan = 0;
                            txtOutput->AppendText("Failed\n");
                        }
                    }
                    else
                        dbentry.cartscan = 0;
                }
                
                if (dbentry.faqs)
                {
                    if (s->get_faqs && 0)
                    //if (s->get_faqs)
                    {
                        txtOutput->AppendText("    Get FAQs: ");
                        char szTempURL[0x1000];
                        char szTempDest[0x1000];
                        sprintf(szTempDest, "%s%04d.zip", szDestFAQPath, dbentry.faqs);
                        bool gotit = false;
                        if (hasLocalCache)
                        {
                            sprintf(szTempURL, "local_library/FAQs/%04d.zip", dbentry.faqs);
                            gotit = getFile(szTempURL, szTempDest);
                        }
                        if (!gotit)
                        {
                            sprintf(szTempURL, "%sFAQs/%04d.zip", s->library_url, dbentry.faqs);
                            gotit = getFile(szTempURL, szTempDest);
                        }
                        if (gotit)
                            txtOutput->AppendText("OK\n");
                        else
                        {
                            dbentry.faqs = 0;
                            txtOutput->AppendText("Failed\n");
                        }
                    }
                    else
                        dbentry.faqs = 0;
                }
                
                if (dbentry.titleshot)
                {
                    if (s->get_titles)
                    {
                        txtOutput->AppendText("    Get Title Shot: ");
                        char szTempURL[0x1000];
                        char szTempDest[0x1000];
                        sprintf(szTempDest, "%s%04d.png", szDestTitleShotsPath, dbentry.titleshot);
                        bool gotit = false;
                        if (hasLocalCache)
                        {
                            sprintf(szTempURL, "local_library/TitleShots/%04d.png", dbentry.titleshot);
                            gotit = getFile(szTempURL, szTempDest);
                        }
                        if (!gotit)
                        {
                            sprintf(szTempURL, "%sTitleShots/%04d.png", s->library_url, dbentry.titleshot);
                            gotit = getFile(szTempURL, szTempDest);
                        }
                        if (gotit)
                            txtOutput->AppendText("OK\n");
                        else
                        {
                            dbentry.titleshot = 0;
                            txtOutput->AppendText("Failed\n");
                        }
                    }
                    else
                        dbentry.titleshot = 0;
                }
                
                if (dbentry.screenshot)
                {
                    if (s->get_screenshots)
                    {
                        txtOutput->AppendText("    Get Screen Shot: ");
                        char szTempURL[0x1000];
                        char szTempDest[0x1000];
                        sprintf(szTempDest, "%s%04d.png", szDestScreenShotsPath, dbentry.screenshot);
                        bool gotit = false;
                        if (hasLocalCache)
                        {
                            sprintf(szTempURL, "local_library/ScreenShots/%04d.png", dbentry.screenshot);
                            gotit = getFile(szTempURL, szTempDest);
                        }
                        if (!gotit)
                        {
                            sprintf(szTempURL, "%sScreenShots/%04d.png", s->library_url, dbentry.screenshot);
                            gotit = getFile(szTempURL, szTempDest);
                        }
                        if (gotit)
                            txtOutput->AppendText("OK\n");
                        else
                        {
                            dbentry.screenshot = 0;
                            txtOutput->AppendText("Failed\n");
                        }
                    }
                    else
                        dbentry.screenshot = 0;
                }
                
                if (dbentry.manual)
                {
                    if (s->get_manuals)
                    {
                        txtOutput->AppendText("    Get Manual: ");
                        char tempdir[0x1000];
                        sprintf(tempdir, "%s%04d/", szDestManualsPath, dbentry.manual);
                        mkdir(tempdir);

                        int page = 0;
                        while (1)
                        {
                            char szTempURL[0x1000];
                            char szTempDest[0x1000];
                            sprintf(szTempDest, "%s%02d.jpg", tempdir, page);
                            bool gotit = false;
                            if (hasLocalCache)
                            {
                                sprintf(szTempURL, "local_library/Manuals/%04d/%02d.jpg", dbentry.manual, page);
                                gotit = getFile(szTempURL, szTempDest);
                            }
                            if (!gotit)
                            {
                                sprintf(szTempURL, "%sManuals/%04d/%02d.jpg", s->library_url, dbentry.manual, page);
                                gotit = getFile(szTempURL, szTempDest);
                            }
                            if (!gotit)
                            {
                                if (page == 0)
                                {
                                    dbentry.manual = 0;
                                    txtOutput->AppendText("Falied\n");
                                }
                                break;
                            }
                            page++;
                        }
                        if (page > 0)
                            txtOutput->AppendText("OK\n");
                    }
                    else
                        dbentry.manual = 0;
                }
                
                if (dbentry.nsf)
                {
                    if (s->get_nsfs)
                    {
                        txtOutput->AppendText("    Get NSF: ");
                        char szTempURL[0x1000];
                        char szTempDest[0x1000];
                        sprintf(szTempDest, "%s%04d.zip", szDestNSFPath, dbentry.nsf);
                        bool gotit = false;
                        if (hasLocalCache)
                        {
                            sprintf(szTempURL, "local_library/nsf/%04d.zip", dbentry.nsf);
                            gotit = getFile(szTempURL, szTempDest);
                        }
                        if (!gotit)
                        {
                            sprintf(szTempURL, "%snsf/%04d.zip", s->library_url, dbentry.nsf);
                            gotit = getFile(szTempURL, szTempDest);
                        }
                        if (gotit)
                            txtOutput->AppendText("OK\n");
                        else
                        {
                            dbentry.nsf = 0;
                            txtOutput->AppendText("Failed\n");
                        }
                    }
                    else
                        dbentry.nsf = 0;
                }
                
                if (dbentry.boxscan)
                {
                    if (s->get_boxes)
                    {
                        txtOutput->AppendText("    Get Box Scan: ");
                        char szTempURL[0x1000];
                        char szTempDest[0x1000];
                        sprintf(szTempDest, "%s%04d.jpg", szDestBoxScansPath, dbentry.boxscan);
                        bool gotit = false;
                        if (hasLocalCache)
                        {
                            sprintf(szTempURL, "local_library/BoxScans/%04d.jpg", dbentry.boxscan);
                            gotit = getFile(szTempURL, szTempDest);
                        }
                        if (!gotit)
                        {
                            sprintf(szTempURL, "%sBoxScans/%04d.jpg", s->library_url, dbentry.boxscan);
                            gotit = getFile(szTempURL, szTempDest);
                        }
                        if (gotit)
                            txtOutput->AppendText("OK\n");
                        else
                        {
                            dbentry.boxscan = 0;
                            txtOutput->AppendText("Failed\n");
                        }
                    }
                    else
                        dbentry.boxscan = 0;
                }
                
                if (dbentry.gamegenie)
                {
                    if (s->get_gamegenie)
                    {
                        txtOutput->AppendText("    Get Game Genie Codes: ");
                        char szTempURL[0x1000];
                        char szTempDest[0x1000];
                        sprintf(szTempDest, "%s%04d.xml", szDestGameGeniePath, dbentry.gamegenie);
                        bool gotit = false;
                        if (hasLocalCache)
                        {
                            sprintf(szTempURL, "local_library/GameGenie/%04d.xml", dbentry.gamegenie);
                            gotit = getFile(szTempURL, szTempDest);
                        }
                        if (!gotit)
                        {
                            sprintf(szTempURL, "%sGameGenie/%04d.xml", s->library_url, dbentry.gamegenie);
                            gotit = getFile(szTempURL, szTempDest);
                        }
                        if (gotit)
                            txtOutput->AppendText("OK\n");
                        else
                        {
                            dbentry.gamegenie = 0;
                            txtOutput->AppendText("Failed\n");
                        }
                    }
                    else
                        dbentry.gamegenie = 0;
                }
                
                NESDB_Add(&myNesDB, &dbentry);
                
                curROMnum++;
            }
            else
            {
                txtOutput->AppendText("\nerror: ");
                txtOutput->AppendText(sztemp);
                txtOutput->AppendText(": not found in db\n");
            }
        }
        else
        {
            txtOutput->AppendText("\nerror: ");
            txtOutput->AppendText(sztemp);
            txtOutput->AppendText(": file not found\n");
        }
    }
    closedir(dp);
    NESDB_Close(&nesdb);
    NESDB_Close(&myNesDB);
    
    
    
    delete s;
    if (TestDestroy())
        return 0;
    
    txtOutput->AppendText("First making ISO... ");
    wxRemoveFile("data.iso");
    wxExecute("mkisofs -V NesterDCSE -l -o data.iso datatemp", wxEXEC_SYNC);
    txtOutput->AppendText("OK!\n");
    wxThread::Sleep(1000);
    if (TestDestroy())
        return 0;
    wxRemoveFile("image.cdi");
    txtOutput->AppendText("Now making CDI from ISO... ");
    wxExecute("bin2boot data.iso /nohack", wxEXEC_SYNC);
    txtOutput->AppendText("OK!\n");
    wxThread::Sleep(1000);
    if (TestDestroy())
        return 0;
    
    if (s->image_format == 0)
    {
        txtOutput->AppendText("Finally making NRG from CDI... ");
        wxRemoveFile("image.nrg");
        wxExecute("cdi2nero image.cdi", wxEXEC_SYNC);
        wxThread::Sleep(1000);
        txtOutput->AppendText("OK!\n");
        wxProcess* proc = wxProcess::Open("cmd /C image.nrg");
        proc->Detach();
    }
    else
    {
        wxProcess* proc = wxProcess::Open("cmd /C image.cdi");
        proc->Detach();
    }
    txtOutput->AppendText("CD build process completed!!!\nLaunching appropriate cd burning program.\n\n");
    
    wxRemoveFile("tmp_cdi_header.tmp");
    wxRemoveFile("tmp_ip.tmp");
    wxRemoveFile("tmp_records.tmp");
    wxRemoveFile("tmplasttrack.tmp");
    wxRemoveFile("data.iso");
    
    dlg->OnBuildComplete();
    return 0;
}
