#ifndef _GLOBAL_OBJECT_H_
#define _GLOBAL_OBJECT_H_

#include "../kjs/value.h"
#include "../kjs/object.h"
#include "../kjs/types.h"
#include "../kjs/interpreter.h"

using namespace KJS;

class GlobalFunctionImp;

class GlobalImp : public ObjectImp
{
public:
	GlobalImp();

	virtual UString className() const { return "Global"; }
	
	static const ClassInfo info;
};

class GlobalFunctionImp : public ObjectImp
{
public:
	GlobalFunctionImp(ExecState *exec, int i, int len);
	
	enum { E_toString, E_print, E_println, E_include, E_includeOnce, E_packColor };
	
	virtual bool implementsCall() const { return true; }
	virtual Value call(ExecState* exec, Object &thisObj, const List &args);
protected:
	uint32 id;
};

void InitGlobal(ExecState* exec, Object obj);

#endif
