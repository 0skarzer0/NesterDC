#include "dreamscript.h"

#include "dsconfig.h"

#include "core/input_object.h"
#include "core/controller_object.h"
#include "core/mouse_object.h"
#include "core/image_object.h"
#include "core/sprite_object.h"
#include "core/global_object.h"
#include "core/video_object.h"
#include "core/system_object.h"
#include "core/filesystem_object.h"
#include "core/file_object.h"
#ifdef MUSIC_SUPPORT
#include "core/music_object.h"
#endif
#include "core/soundeffect_object.h"
#include "core/font_object.h"
#include "core/zipfile_object.h"
#include "core/xml_object.h"
#include "core/vmufile_object.h"

using namespace KJS;

Object* g_global = NULL;
Interpreter* g_interp = NULL;

bool DreamScript_Init()
{
	g_global = new Object(new GlobalImp());
	g_interp = new Interpreter(*g_global);
	ExecState* exec = g_interp->globalExec();
	
	InitGlobal(exec, *g_global);
	InitInputObject(exec, *g_global);
	InitControllers(exec, Object(g_inputImp));
	InitMice(exec, *g_global);
	InitVideoObject(exec, *g_global);
	InitSystemObject(exec, *g_global);
	InitFileSystemObject(exec, *g_global);
	InitFileClass(exec, *g_global);
	InitImageClass(exec, *g_global);
	InitSpriteClass(exec, *g_global);
	InitFontClass(exec, *g_global);
#ifdef MUSIC_SUPPORT
	InitMusicObject(exec, *g_global);
#endif
	InitSoundEffectClass(exec, *g_global);
	InitZipFileClass(exec, *g_global);
	InitXMLObject(exec, *g_global);
	InitVMUFileClass(exec, *g_global);
	
	return true;
}

void DreamScript_Shutdown()
{
	
}

bool DreamScript_Evaluate(const char* str, const char* codename)
{
	Completion comp(g_interp->evaluate(str, Value(), codename));
	return true;
}

bool DreamScript_EvaluateFile(const char* filename)
{
	FILE* fp = fopen(filename, "rb");
	fseek(fp, 0, SEEK_END);
	uint32 filesize = ftell(fp);
	fseek(fp, 0, SEEK_SET);
	char* code = new char[filesize + 1];
	fread(code, 1, filesize, fp);
	code[filesize] = '\0';
	fclose(fp);
	bool result = DreamScript_Evaluate(code, filename);
	delete[] code;
	return result;
}

Object* DreamScript_GetGlobalObject()
{
	return g_global;
}

Interpreter* DreamScript_GetInterpreter()
{
	return g_interp;
}
