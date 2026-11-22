#ifndef _INPUT_OBJECT_H_
#define _INPUT_OBJECT_H_

#include "../kjs/value.h"
#include "../kjs/object.h"
#include "../kjs/types.h"
#include "../kjs/interpreter.h"
#include "punk_array.h"
#include "XML.h"

using namespace KJS;

class InputFunctionImp;

class InputImp : public ObjectImp
{
public:
	InputImp();
	
	enum { E_toString, E_scanDevices };

	virtual UString className() const { return "Input"; }
	virtual Value get(ExecState *exec, const Identifier &propertyName) const;
    Value getValueProperty(ExecState *exec, int token) const;
	
	//JS
	void rescanDevices();
	
	static const ClassInfo info;
};

class InputFunctionImp : public ObjectImp
{
public:
	InputFunctionImp(ExecState *exec, int i, int len);
	
	virtual bool implementsCall() const { return true; }
	virtual Value call(ExecState* exec, Object &thisObj, const List &args);
protected:
	uint32 id;
};

void InitInputObject(ExecState* exec, Object obj);

extern InputImp* g_inputImp;

extern struct SXML* g_inputXML;

#endif
