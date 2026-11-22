#ifndef _DC_FILE_BROWSER_H_
#define _DC_FILE_BROWSER_H_

#include <kos.h>
#include "dc_menu.h"
#include "punk_array.h"
#include "types.h"

struct DCFileBrowser
{
	uint32 directory_color;
	uint8 deep;
	bool8 found;
	struct DCMenu menu;
	char dir[0x1000];
	char highest_dir[0x1000];
	char file_path[0x1000];
	char file_name[0x100];
	bool8 has_old_data;
	struct SPVRTexture* default_icon;
	struct SPVRTexture* folder_icon;
	DECLARE_PUNK_ARRAY(icons, struct SPVRTexture*);
	DECLARE_PUNK_ARRAY(icon_extensions, char*);
};

START_EXTERN_C

void DCFileBrowser_Init(struct DCFileBrowser* p);
void DCFileBrowser_Destroy(struct DCFileBrowser* p);
bool8 DCFileBrowser_Browse(struct DCFileBrowser* p, bool8 use_last /*= FALSE*/);
void DCFileBrowser_SetDirectory(struct DCFileBrowser* p, const char* szdir);
const char* DCFileBrowser_GetDirectory(struct DCFileBrowser* p);
void DCFileBrowser_SetHighestDirecty(struct DCFileBrowser* p, const char* szdir);
const char* DCFileBrowser_GetHighestDirecty(struct DCFileBrowser* p);
const char* DCFileBrowser_GetChosenFilePath(struct DCFileBrowser* p);
const char* DCFileBrowser_GetChosenFileName(struct DCFileBrowser* p);
void DCFileBrowser_SetIcon(struct DCFileBrowser* p, struct SPVRTexture* icon, const char* extension);

// DCMenu event handlers, do not call
void DCFileBrowser_OnChooseDir(struct DCMenu* pMenu, struct DCMenuItem* pMenuItem, int value);
void DCFileBrowser_OnChooseFile(struct DCMenu* pMenu, struct DCMenuItem* pMenuItem, int value);

END_EXTERN_C

#endif

