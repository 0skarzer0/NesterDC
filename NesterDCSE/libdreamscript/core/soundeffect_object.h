#ifndef _SOUNDEFFECT_OBJECT_H_
#define _SOUNDEFFECT_OBJECT_H_

#include <kos.h>
#include "../kjs/value.h"
#include "../kjs/internal.h"
#include "../kjs/function_object.h"
#include "image.h"
#include "hashmap.h"

//#include <sys/time.h>
#include "../kjs/port_kjs.h"

using namespace KJS;

class SoundEffectInstanceImp : public ObjectImp
{
public:
	SoundEffectInstanceImp(ObjectImp* proto);
	virtual ~SoundEffectInstanceImp();

	enum { E_toString, E_load, E_unload, E_play, E_stop };

	virtual const ClassInfo* classInfo() const { return &info; }
	virtual Value get(ExecState *exec, const Identifier &propertyName) const;
    Value getValueProperty(ExecState *exec, int token) const;
	void put(ExecState *exec, const Identifier &propertyName, const Value &value, int attr = None);
    void putValueProperty(ExecState *exec, int token, const Value &value, int attr = None);
	
	// Start JS Functions
	CString toString();
	bool load(const char* filename);
	void unload();
	void play(int vol, int pan);
	void stop();
	// End JS Functions
	
	static const ClassInfo info;
	
protected:
	sfxhnd_t hSFX;
	int channel;
	
	static uint64 channel_usage;
};

class SoundEffectPrototypeImp : public SoundEffectInstanceImp
{
public:
	SoundEffectPrototypeImp(ExecState* exec, ObjectPrototypeImp* objectProto);
	Value get(ExecState *exec, const Identifier &p) const;
	virtual const ClassInfo* classInfo() const { return &info; }
	static const ClassInfo info;
};

class SoundEffectProtoFuncImp : public InternalFunctionImp
{
public:
	SoundEffectProtoFuncImp(ExecState* exec, int i, int len);

	virtual bool implementsCall() const { return true; }
	virtual Value call(ExecState* exec, Object& thisObj, const List& args);
private:
	int id;
};

class SoundEffectObjectImp : public InternalFunctionImp
{
public:
	SoundEffectObjectImp(ExecState* exec, FunctionPrototypeImp* funcProto, SoundEffectPrototypeImp* dateProto);

	virtual bool implementsConstruct() const { return true; }
	virtual Object construct(ExecState* exec, const List &args);

	Object construct(const List &);
};

void InitSoundEffectClass(ExecState* exec, Object globobj);

#endif
