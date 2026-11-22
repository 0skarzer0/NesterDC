#ifndef _MUSIC_OBJECT_H_
#define _MUSIC_OBJECT_H_

#include "../dsconfig.h"

#ifdef MUSIC_SUPPORT

#include <kos.h>
#include <mp3/sndserver.h>

#include "../kjs/value.h"
#include "../kjs/object.h"
#include "../kjs/types.h"
#include "../kjs/interpreter.h"
#include "punk_array.h"

using namespace KJS;

#define MUSIC_TYPE_NONE		0
#define MUSIC_TYPE_MP3		1
#define MUSIC_TYPE_OGG		2
#define MUSIC_TYPE_MOD		3

extern snd_stream_hnd_t music_stream_hnd;

class MusicImp : public ObjectImp
{
public:
	MusicImp();
	
	enum { E_toString, E_init, E_shutdown, E_play, E_stop };

	virtual UString className() const { return "Music"; }
	virtual Value get(ExecState *exec, const Identifier &propertyName) const;
    Value getValueProperty(ExecState *exec, int token) const;
    
    bool init();
    bool shutdown();
	bool play(const char* fn);
	bool stop();
	
	static const ClassInfo info;
	
	int playing_type;
	
	static bool initted;
};

class MusicFunctionImp : public ObjectImp
{
public:
	MusicFunctionImp(ExecState *exec, int i, int len);
	
	virtual bool implementsCall() const { return true; }
	virtual Value call(ExecState* exec, Object &thisObj, const List &args);
protected:
	uint32 id;
};

void InitMusicObject(ExecState* exec, Object obj);

extern MusicImp* g_musicImp;

#endif //MUSIC_SUPPORT

#endif
