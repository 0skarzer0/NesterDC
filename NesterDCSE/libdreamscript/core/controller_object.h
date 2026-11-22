#ifndef _CONTROLLER_OBJECT_H_
#define _CONTROLLER_OBJECT_H_

#include "../kjs/value.h"
#include "../kjs/object.h"
#include "../kjs/types.h"
#include "../kjs/interpreter.h"
#include "dc_controller.h"
#include "hashmap.h"
#include "XML.h"

using namespace KJS;

class ControllerFunctionImp;

class ControllerImp : public ObjectImp
{
public:
	ControllerImp(struct DCController* pad);
	
	enum { e_poll, e_setMode, e_getMode, e_justPressed };

	virtual UString className() const { return "Controller"; }
	virtual Value get(ExecState *exec, const Identifier &propertyName) const;
    //Value getValueProperty(ExecState *exec, int token) const;
	
	// JS Funcs
	void poll();
	bool setMode(const char* strMapName);
	CString getMode();
	bool justPressed(const char* strval);
	// end JS Funcs
	
	struct DCController* m_pad;
	struct hashmap* cmdMap;
	CString modeName;
	
	static const ClassInfo info;
	static struct hashmap* storedMaps;
	static bool InitDefaultMappings();
	static bool LoadMappingsXML(struct SXML* xmlnode);
};

class ControllerFunctionImp : public ObjectImp
{
public:
	ControllerFunctionImp(ExecState *exec, int i, int len);
	
	virtual bool implementsCall() const { return true; }
	virtual Value call(ExecState* exec, Object &thisObj, const List &args);
protected:
	uint32 id;
};

void InitControllers(ExecState* exec, Object obj);

#endif
