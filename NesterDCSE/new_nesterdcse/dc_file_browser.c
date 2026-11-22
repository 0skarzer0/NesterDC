#include "dc_file_browser.h"

void DCFileBrowser_Init(struct DCFileBrowser* p)
{
	DCMenu_Init(&p->menu, NULL, NULL);
	
	strcpy(p->dir, "/cd");
	p->menu.user_data = p;
	p->found = FALSE;
	p->deep = 0;
	p->file_path[0] = '\0';
	p->file_name[0] = '\0';
	strcpy(p->highest_dir, "/cd");
	p->has_old_data = FALSE;
	p->default_icon = NULL;
	p->folder_icon = NULL;
	p->directory_color = 0x11000000;
	punk_array_init(p->icons);
	punk_array_init(p->icon_extensions);
}

void DCFileBrowser_Destroy(struct DCFileBrowser* p)
{
	uint32 k;
	for (k = 0; k < p->icons.count; ++k)
		free(p->icons.data[k]);
}

bool8 DCFileBrowser_Browse(struct DCFileBrowser* p, bool8 use_last)
{
	p->found = FALSE;
	bool8 on_highest;
	while (!p->found)
	{
		on_highest = (strcmp(p->highest_dir, p->dir) == 0);
		if (!p->has_old_data || !use_last)
		{
			DCMenu_RemoveAllItems(&p->menu);
			++p->deep;
			file_t fpd = fs_open(p->dir, O_DIR | O_RDONLY);
			if (!fpd)
				return FALSE;
			dirent_t* dirinfo;
			struct DCMenuItem* itemtemp = NULL;
			char* dotpos = NULL;
			bool8 found_ext = FALSE;
			while ((dirinfo = fs_readdir(fpd)))
			{
				if (dirinfo->size == -1)
				{
					itemtemp = DCMenu_AddItem(&p->menu, dirinfo->name, DCFileBrowser_OnChooseDir);
					itemtemp->color = p->directory_color;
					if (p->folder_icon)
						itemtemp->icon_tex = p->folder_icon;
				}
				else
					itemtemp = DCMenu_AddItem(&p->menu, dirinfo->name, DCFileBrowser_OnChooseFile);
				dotpos = strrchr(dirinfo->name, '.');
				if (dotpos)
				{
					found_ext = FALSE;
					uint32 k;
					for (k = 0; k < p->icon_extensions.count; ++k)
					{
						if (stricmp(dotpos, p->icon_extensions.data[k]) == 0)
						{
							itemtemp->icon_tex = p->default_icon;
							found_ext = TRUE;
							break;
						}
					}
					if (!found_ext && p->default_icon)
						itemtemp->icon_tex = p->default_icon;
				}
				else if (p->default_icon)
					itemtemp->icon_tex = p->default_icon;
			}
			fs_close(fpd);
			p->has_old_data = TRUE;
		}
		use_last = FALSE;
		if (!DCMenu_Run(&p->menu))
		{
			if (on_highest)
				break;
			char* pSlash = strrchr(p->dir, '/');
			if (pSlash == NULL)
				break;
			*pSlash = '\0';
		}
	}
	return p->found;
}

void DCFileBrowser_SetDirectory(struct DCFileBrowser* p, const char* szdir)
{
	strcpy(p->dir, szdir);
}

const char* DCFileBrowser_GetDirectory(struct DCFileBrowser* p)
{
	if (strlen(p->dir) == 0)
		return NULL;
	return p->dir;
}

void DCFileBrowser_SetHighestDirecty(struct DCFileBrowser* p, const char* szdir)
{
	strcpy(p->highest_dir, szdir);
}

const char* DCFileBrowser_GetHighestDirecty(struct DCFileBrowser* p)
{
	if (strlen(p->highest_dir) == 0)
		return NULL;
	return p->highest_dir;
}

const char* DCFileBrowser_GetChosenFileName(struct DCFileBrowser* p)
{
	if (strlen(p->file_name) == 0)
		return NULL;
	return p->file_name;
}

const char* DCFileBrowser_GetChosenFilePath(struct DCFileBrowser* p)
{
	if (strlen(p->file_path) == 0)
		return NULL;
	return p->file_path;
}

void DCFileBrowser_SetIcon(struct DCFileBrowser* p, struct SPVRTexture* icon, const char* extension)
{
	uint32 k;
	for (k = 0; k < p->icon_extensions.count; ++k)
	{
		if (stricmp(p->icon_extensions.data[k], extension) == 0)
		{
			p->icons.data[k] = icon;
			return;
		}
	}
	punk_array_push(p->icons, icon);
	char* sztemp = (char*) malloc(strlen(extension) + 1);
	strcpy(sztemp, extension);
	punk_array_push(p->icon_extensions, sztemp);
}

void DCFileBrowser_OnChooseDir(struct DCMenu* pMenu, struct DCMenuItem* pMenuItem, int value)
{
	struct DCFileBrowser* pBrowser = (struct DCFileBrowser*) pMenu->user_data;
	strcat(pBrowser->dir, "/");
	strcat(pBrowser->dir, pMenuItem->text);
	DCMenu_Stop(&(pBrowser->menu), FALSE);
}

void DCFileBrowser_OnChooseFile(struct DCMenu* pMenu, struct DCMenuItem* pMenuItem, int value)
{
	struct DCFileBrowser* pBrowser = (struct DCFileBrowser*) pMenu->user_data;
	strcpy(pBrowser->file_name, pMenuItem->text);
	sprintf(pBrowser->file_path, "%s/%s", pBrowser->dir, pMenuItem->text);
	pBrowser->found = TRUE;
	DCMenu_Stop(&(pBrowser->menu), FALSE);
}
