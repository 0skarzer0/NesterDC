#include "soundeffect_object.h"
#include "../kjs/error_object.h"
#include "../kjs/operations.h"
#include "../kjs/lookup.h"
#include "punk_array.h"

#include "soundeffect_object.lut.h"

extern "C" {
#include <imageload/imageload.h>
}

using namespace KJS;

Object g_SoundEffectPrototype;
Object g_SoundEffect;

void InitSoundEffectClass(ExecState* exec, Object global)
{
	SoundEffectPrototypeImp* soundeffectProto = new SoundEffectPrototypeImp(exec, (ObjectPrototypeImp*) exec->interpreter()->builtinObjectPrototype().imp());
	g_SoundEffectPrototype = Object(soundeffectProto);
	g_SoundEffect = Object(new SoundEffectObjectImp(exec, (FunctionPrototypeImp*) exec->interpreter()->builtinFunctionPrototype().imp(), soundeffectProto));
	global.put(exec, "SoundEffect", g_SoundEffect, DontEnum|ReadOnly);
}

//INSTANCE

/*

enum { E_toString, E_load, E_unload, E_play, E_stop };

@begin soundeffectTable 5
	toString		SoundEffectInstanceImp::E_toString		DontDelete|DontEnum|ReadOnly|Function	0
	load			SoundEffectInstanceImp::E_load			DontDelete|DontEnum|ReadOnly|Function	1
	unload			SoundEffectInstanceImp::E_unload		DontDelete|DontEnum|ReadOnly|Function	0
	play			SoundEffectInstanceImp::E_play			DontDelete|DontEnum|ReadOnly|Function	2
	stop			SoundEffectInstanceImp::E_stop			DontDelete|DontEnum|ReadOnly|Function	0
	
@end
*/

const ClassInfo SoundEffectInstanceImp::info = {"SoundEffect", 0, 0, 0};

SoundEffectInstanceImp::SoundEffectInstanceImp(ObjectImp* proto)
  : ObjectImp(proto)
{
	hSFX = SFXHND_INVALID;
	channel = -1;
}

SoundEffectInstanceImp::~SoundEffectInstanceImp()
{
	
}

Value SoundEffectInstanceImp::get(ExecState *exec, const Identifier &propertyName) const
{
	return lookupGet<SoundEffectProtoFuncImp, SoundEffectInstanceImp, ObjectImp>(exec, propertyName, &soundeffectTable, this);
}

Value SoundEffectInstanceImp::getValueProperty(ExecState *exec, int token) const
{
	switch (token)
	{
		//case SoundEffectInstanceImp::E_animation:
		//	return String(animation.c_str());
	}
	return Undefined();
}

void SoundEffectInstanceImp::put(ExecState *exec, const Identifier &propertyName, const Value &value, int attr)
{
	lookupPut<SoundEffectInstanceImp, ObjectImp>(exec, propertyName, value, attr, &soundeffectTable, this);
}

void SoundEffectInstanceImp::putValueProperty(ExecState *exec, int token, const Value &value, int attr)
{
	switch (token)
	{
		//case SoundEffectInstanceImp::E_animation:
		//{
		//	break;
		//}
	}
}


CString SoundEffectInstanceImp::toString()
{
	return "SoundEffect object ";
}

bool SoundEffectInstanceImp::load(const char* filename)
{
	unload();
	return (hSFX = snd_sfx_load(filename)) != SFXHND_INVALID;
}

void SoundEffectInstanceImp::unload()
{
	if (hSFX != SFXHND_INVALID)
	{
		snd_sfx_unload(hSFX);
		hSFX = SFXHND_INVALID;
		channel = -1;
	}
}

void SoundEffectInstanceImp::play(int vol, int pan)
{
	if (hSFX != SFXHND_INVALID)
		channel = snd_sfx_play(hSFX, vol, pan);
}

void SoundEffectInstanceImp::stop()
{
	if (channel != -1)
		snd_sfx_stop(channel);
}

// PROTOTYPE

const ClassInfo SoundEffectPrototypeImp::info = {"SoundEffect", 0, &soundeffectTable, 0};

SoundEffectPrototypeImp::SoundEffectPrototypeImp(ExecState*, ObjectPrototypeImp* objectProto)
  : SoundEffectInstanceImp(objectProto)
{
	Value protect(this);
	setInternalValue(Null());
}

Value SoundEffectPrototypeImp::get(ExecState* exec, const Identifier& propertyName) const
{
	return lookupGetFunction<SoundEffectProtoFuncImp, ObjectImp>(exec, propertyName, &soundeffectTable, this);
}

SoundEffectProtoFuncImp::SoundEffectProtoFuncImp(ExecState* exec, int i, int len)
  : InternalFunctionImp(
    static_cast<FunctionPrototypeImp*>(exec->interpreter()->builtinFunctionPrototype().imp())
    ), id(abs(i))
{
	Value protect(this);
	putDirect(lengthPropertyName, len, DontDelete|ReadOnly|DontEnum);
}

Value SoundEffectProtoFuncImp::call(ExecState* exec, Object& thisObj, const List &args)
{
	if (thisObj.className() != "SoundEffect")
	{
		Object err = Error::create(exec, TypeError);
		exec->setException(err);
		return err;
	}
	SoundEffectInstanceImp* sfx = (SoundEffectInstanceImp*) thisObj.imp();
	switch (id)
	{
		case SoundEffectInstanceImp::E_toString:
			return String(sfx->toString().c_str());
		//enum { E_toString, E_load, E_play, E_stop };
		case SoundEffectInstanceImp::E_load:
		{
			if (args.size() > 0)
			{
				CString str = args[0].toString(exec).cstring();
				return Boolean(sfx->load(str.c_str()));
			}
			break;
		}
		case SoundEffectInstanceImp::E_unload:
		{
			sfx->unload();
			break;
		}
		case SoundEffectInstanceImp::E_play:
		{
			switch (args.size())
			{
				case 2:
					sfx->play(args[0].toInteger(exec), args[1].toInteger(exec));
					break;
				case 1:
					sfx->play(args[0].toInteger(exec), 127);
					break;
				default:
					sfx->play(255, 127);
					break;
			};
			break;
		}
		case SoundEffectInstanceImp::E_stop:
			sfx->stop();
			break;
	}
	return Undefined();
}

//OBJECT

SoundEffectObjectImp::SoundEffectObjectImp(ExecState* exec, FunctionPrototypeImp* funcProto, SoundEffectPrototypeImp* soundeffectProto)
  : InternalFunctionImp(funcProto)
{
  Value protect(this);
  
  putDirect(prototypePropertyName, soundeffectProto, DontEnum|DontDelete|ReadOnly);

  // no. of arguments for constructor
  putDirect(lengthPropertyName, 0, ReadOnly|DontDelete|DontEnum);
}

Object SoundEffectObjectImp::construct(ExecState* exec, const List& args)
{
	return Object(new SoundEffectInstanceImp(g_SoundEffectPrototype.imp()));
}

