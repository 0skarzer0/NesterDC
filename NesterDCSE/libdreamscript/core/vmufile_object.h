#ifndef _VMUFILE_OBJECT_H_
#define _VMUFILE_OBJECT_H_

#include "../kjs/value.h"
#include "../kjs/internal.h"
#include "../kjs/function_object.h"

#include "../kjs/port_kjs.h"

using namespace KJS;

class VMUFileInstanceImp : public ObjectImp
{
public:
	VMUFileInstanceImp(ObjectImp* proto);
	~VMUFileInstanceImp();
	
	enum { E_shortDesc, E_longDesc, E_appID, E_data };

	virtual const ClassInfo* classInfo() const { return &info; }
	virtual Value get(ExecState *exec, const Identifier &propertyName) const;
	Value getValueProperty(ExecState *exec, int token) const;
	void put(ExecState *exec, const Identifier &propertyName, const Value &value, int attr = None);
    void putValueProperty(ExecState *exec, int token, const Value &value, int attr = None);
	
	static const ClassInfo info;
	
	// Start JS Functions
	CString toString();
	
	// End JS Functions
	
	vmu_pkg_t pkg;
};

class VMUFilePrototypeImp : public VMUFileInstanceImp
{
public:
	VMUFilePrototypeImp(ExecState* exec, ObjectPrototypeImp* objectProto);
	Value get(ExecState *exec, const Identifier &p) const;
	virtual const ClassInfo* classInfo() const { return &info; }
	static const ClassInfo info;
};

class VMUFileProtoFuncImp : public InternalFunctionImp
{
public:
	VMUFileProtoFuncImp(ExecState* exec, int i, int len);

	virtual bool implementsCall() const { return true; }
	virtual Value call(ExecState* exec, Object& thisObj, const List& args);

	enum { E_toString, E_save, E_load };
private:
	int id;
};

class VMUFileObjectImp : public InternalFunctionImp
{
public:
	VMUFileObjectImp(ExecState* exec, FunctionPrototypeImp* funcProto, VMUFilePrototypeImp* fileProto);

	virtual bool implementsConstruct() const { return true; }
	virtual Object construct(ExecState* exec, const List &args);

	Object construct(const List &);
};

void InitVMUFileClass(ExecState* exec, Object globobj);

extern Object g_VMUFilePrototype;
extern Object g_VMUFile;

#endif
