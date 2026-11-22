#ifndef _SYSTEM_OBJECT_H_
#define _SYSTEM_OBJECT_H_

#include "../kjs/value.h"
#include "../kjs/object.h"
#include "../kjs/types.h"
#include "../kjs/interpreter.h"
#include "punk_array.h"

using namespace KJS;

class SystemFunctionImp;

class SystemImp : public ObjectImp
{
public:
	SystemImp();

	enum { E_toString, E_availableMemory, E_garbageCollect, E_startTimer, E_stopTimer };

	virtual UString className() const { return "System"; }
	virtual Value get(ExecState *exec, const Identifier &propertyName) const;
    Value getValueProperty(ExecState *exec, int token) const;

	static const ClassInfo info;

private:
};

class SystemFunctionImp : public ObjectImp
{
public:
	SystemFunctionImp(ExecState *exec, int i, int len);

	virtual bool implementsCall() const { return true; }
	virtual Value call(ExecState* exec, Object &thisObj, const List &args);
protected:
	uint32 id;
};

void InitSystemObject(ExecState* exec, Object obj);

extern SystemImp* g_systemImp;

#endif
