#ifndef _NESDB_OBJECT_H_
#define _NESDB_OBJECT_H_

#include <kjs/value.h>
#include <kjs/internal.h>
#include <kjs/function_object.h>

#include "nesdb.h"

using namespace KJS;

class NESDBInstanceImp : public ObjectImp
{
public:
	NESDBInstanceImp(ObjectImp* proto);
	virtual ~NESDBInstanceImp();

	enum { E_toString, E_open, E_close, E_get, E_size };

	virtual const ClassInfo* classInfo() const { return &info; }
	virtual Value get(ExecState *exec, const Identifier &propertyName) const;
    Value getValueProperty(ExecState *exec, int token) const;
	
	static const ClassInfo info;
	
	struct NES_DB nesdb;
};

class NESDBPrototypeImp : public NESDBInstanceImp
{
public:
	NESDBPrototypeImp(ExecState* exec, ObjectPrototypeImp* objectProto);
	Value get(ExecState *exec, const Identifier &p) const;
	virtual const ClassInfo* classInfo() const { return &info; }
	static const ClassInfo info;
};

class NESDBProtoFuncImp : public InternalFunctionImp
{
public:
	NESDBProtoFuncImp(ExecState* exec, int i, int len);

	virtual bool implementsCall() const { return true; }
	virtual Value call(ExecState* exec, Object& thisObj, const List& args);
private:
	int id;
};

class NESDBObjectImp : public InternalFunctionImp
{
public:
	NESDBObjectImp(ExecState* exec, FunctionPrototypeImp* funcProto, NESDBPrototypeImp* nesdbProto);

	virtual bool implementsConstruct() const { return true; }
	virtual Object construct(ExecState* exec, const List &args);

	Object construct(const List &);
};

void InitNESDBClass(ExecState* exec, Object globobj);

#endif
