#ifndef _XML_OBJECT_H_
#define _XML_OBJECT_H_

#include "../kjs/value.h"
#include "../kjs/object.h"
#include "../kjs/types.h"
#include "../kjs/interpreter.h"
#include "XML.h"

using namespace KJS;

class XMLImp : public ObjectImp
{
public:
	XMLImp();
	
	enum { E_toString, E_file2Obj, E_str2Obj };

	virtual UString className() const { return "XML"; }
	virtual Value get(ExecState *exec, const Identifier &propertyName) const;
    Value getValueProperty(ExecState *exec, int token) const;
	void put(ExecState *exec, const Identifier &propertyName, const Value &value, int attr = None);
    void putValueProperty(ExecState *exec, int token, const Value &value, int attr = None);
	
	static const ClassInfo info;
	
private:
};

class XMLFunctionImp : public ObjectImp
{
public:
	XMLFunctionImp(ExecState *exec, int i, int len);
	
	virtual bool implementsCall() const { return true; }
	virtual Value call(ExecState* exec, Object &thisObj, const List &args);
protected:
	uint32 id;
};

void InitXMLObject(ExecState* exec, Object obj);

extern XMLImp* g_xmlImp;

#endif
