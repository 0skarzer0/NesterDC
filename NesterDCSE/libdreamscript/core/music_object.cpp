#include "../dsconfig.h"

#ifdef MUSIC_SUPPORT

#include "music_object.h"
#include "../kjs/lookup.h"
#include "music_object.lut.h"

#include "dc_utils.h"

#include <oggvorbis/sndoggvorbis.h>
#include <modplug/modplug.h>

const ClassInfo MusicImp::info = { "Music", 0, &musicTable, 0 };

MusicImp* g_musicImp = NULL;

#define MOD_BUF_SIZE	0x2000
snd_stream_hnd_t music_stream_hnd;
uint8 sound_buf[MOD_BUF_SIZE] __attribute__ ((aligned (32)));
ModPlugFile* modplug_file = NULL;
ModPlug_Settings modplug_settings;

/* Source for music_object.lut.h

enum { E_toString, E_init, E_shutdown, E_play, E_stop };

@begin musicTable 4
	toString		MusicImp::E_toString		DontDelete|ReadOnly|Function	0
	init			MusicImp::E_init			DontDelete|ReadOnly|Function	0
	shutdown		MusicImp::E_shutdown		DontDelete|ReadOnly|Function	0
	play			MusicImp::E_play			DontDelete|ReadOnly|Function	1
	stop			MusicImp::E_stop			DontDelete|ReadOnly|Function	0
@end
*/

MusicImp::MusicImp() : ObjectImp()
{
	playing_type = MUSIC_TYPE_NONE;
}

bool MusicImp::initted = false;

bool MusicImp::init()
{
	if (!MusicImp::initted)
	{
		snd_stream_init();
		MusicImp::initted = true;
	}
	return MusicImp::initted;
}

bool MusicImp::shutdown()
{
	if (MusicImp::initted)
	{
		stop();
		snd_stream_shutdown();
		MusicImp::initted = false;
	}
	return !MusicImp::initted;
}

//bool first_kjsMODGetSound = false;

void* kjsMODGetSound(snd_stream_hnd_t streamHnd, int samples_requested, int* samples_returned)
{/*
	if (first_kjsMODGetSound)
	{
		*samples_returned = samples_requested;
		first_kjsMODGetSound = false;
		return sound_buf;
	}*/
	
	*samples_returned = ModPlug_Read(modplug_file, sound_buf, samples_requested);
	return sound_buf;
}

static kthread_t * thd = NULL;

static volatile bool keepRunningThread = false;

static void mod_sndserver_thread(void *blagh)
{
	while (keepRunningThread)
	{
		snd_stream_poll(music_stream_hnd);
		thd_sleep(15);
	}
}

bool MusicImp::play(const char* fn)
{
	stop();
	char* dotpos = strrchr(fn, '.');
	if (stricmp(dotpos, ".mp3") == 0)
	{
		mp3_init();
		mp3_start(fn,0);
		playing_type = MUSIC_TYPE_MP3;
		return true;
	}
	else if (stricmp(dotpos, ".ogg") == 0)
	{
		sndoggvorbis_init();
		dbgio_printf("%s\n", fn);
		sndoggvorbis_start(fn,0);
		playing_type = MUSIC_TYPE_OGG;
		return true;
	}
	else if (stricmp(dotpos, ".xm") == 0
			 || stricmp(dotpos, ".wav") == 0
			 || stricmp(dotpos, ".s3m") == 0
			 || stricmp(dotpos, ".mod") == 0
			 || stricmp(dotpos, ".it") == 0)
	{
		FILE* fp = fopen(fn, "rb");
		if (!fp)
			return false;
		fseek(fp, 0, SEEK_END);
		int fsize = ftell(fp);
		fseek(fp, 0, SEEK_SET);
		uint8* mod_data = new uint8[fsize];
		fread(mod_data, sizeof(uint8), fsize, fp);
		fclose(fp);
		
		ModPlug_Settings modSettings;
		ModPlug_GetSettings(&modSettings);
		modSettings.mLoopCount = 999;
		ModPlug_SetSettings(&modSettings);
		
		modplug_file = ModPlug_Load(mod_data, fsize);
		if (!modplug_file)
		{
			delete[] mod_data;
			return false;
		}
		delete[] mod_data;
		ModPlug_GetSettings(&modplug_settings);
		//dbgio_printf("modplug_settings.mFrequency = %d\n", modplug_settings.mFrequency);
		//dbgio_printf("modplug_settings.mChannels = %d\n", modplug_settings.mChannels);
		
		memset(sound_buf, 0, MOD_BUF_SIZE);
		//first_kjsMODGetSound = true;
		music_stream_hnd = snd_stream_alloc(kjsMODGetSound, MOD_BUF_SIZE);
		snd_stream_start(music_stream_hnd, modplug_settings.mFrequency, modplug_settings.mChannels == 2);
		playing_type = MUSIC_TYPE_MOD;
		
		keepRunningThread = true;
		thd = thd_create(mod_sndserver_thread, NULL);
		
		return true;
	}
	return false;
}

bool MusicImp::stop()
{
	if (playing_type == MUSIC_TYPE_MP3)
	{
		mp3_stop();
		//mp3_quit();
	}
	else if (playing_type == MUSIC_TYPE_OGG)
	{
		sndoggvorbis_stop();
		sndoggvorbis_shutdown();
	}
	else if (playing_type == MUSIC_TYPE_MOD)
	{
		keepRunningThread = false;
		thd_wait(thd);
		thd = NULL;
		snd_stream_stop(music_stream_hnd);
		snd_stream_destroy(music_stream_hnd);
	}
	playing_type = MUSIC_TYPE_NONE;
	return true;
}

Value MusicImp::get(ExecState *exec, const Identifier &propertyName) const
{
	return lookupGet<MusicFunctionImp, MusicImp, ObjectImp>(exec, propertyName, &musicTable, this);
}

Value MusicImp::getValueProperty(ExecState *exec, int token) const
{
	return Undefined();
}

MusicFunctionImp::MusicFunctionImp(ExecState* exec, int i, int len) : ObjectImp()
{
	id = i;
	put(exec, lengthPropertyName, Number(len), DontDelete|ReadOnly|DontEnum);
}

Value MusicFunctionImp::call(ExecState *exec, Object &thisObj, const List &args)
{
	if (thisObj.className() != "Music")
	{
		Object err = Error::create(exec, TypeError);
		exec->setException(err);
		return err;
	}
	switch (id)
	{
		case MusicImp::E_toString:
		{
			return String("Music Object");
			break;
		}
		case MusicImp::E_play:
		{
			if (args.size() == 1)
			{
				CString str = args[0].toString(exec).cstring();
				return Boolean(((MusicImp*) thisObj.imp())->play(str.c_str()));
			}
			break;
		}
		case MusicImp::E_stop:
		{
			return Boolean(((MusicImp*) thisObj.imp())->stop());
			break;
		}
		case MusicImp::E_init:
		{
			return Boolean(((MusicImp*) thisObj.imp())->init());
			break;
		}
		case MusicImp::E_shutdown:
		{
			return Boolean(((MusicImp*) thisObj.imp())->shutdown());
			break;
		}
	}
	return Undefined();
}

void InitMusicObject(ExecState* exec, Object obj)
{
	g_musicImp = new MusicImp();
	obj.put(exec, Identifier("Music"), Object(g_musicImp));
}

#endif //MUSIC_SUPPORT
