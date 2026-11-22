#include "nesdb_object.h"
#include <kjs/error_object.h>
#include <kjs/operations.h>
#include <kjs/lookup.h>

#include "nesdb_object.lut.h"

using namespace KJS;

Object g_NESDBPrototype;
Object g_NESDB;

void InitNESDBClass(ExecState* exec, Object global)
{
	NESDBPrototypeImp* nesdbProto = new NESDBPrototypeImp(exec, (ObjectPrototypeImp*) exec->interpreter()->builtinObjectPrototype().imp());
	g_NESDBPrototype = Object(nesdbProto);
	g_NESDB = Object(new NESDBObjectImp(exec, (FunctionPrototypeImp*) exec->interpreter()->builtinFunctionPrototype().imp(), nesdbProto));
	global.put(exec, "NESDB", g_NESDB, DontEnum|ReadOnly);
}

//INSTANCE

/*

enum { E_toString, E_open, E_close, E_get, E_size };

@begin nesdbTable 6
	toString		NESDBInstanceImp::E_toString			DontDelete|DontEnum|ReadOnly|Function	0
	open			NESDBInstanceImp::E_open				DontDelete|DontEnum|ReadOnly|Function	1
	close			NESDBInstanceImp::E_close				DontDelete|DontEnum|ReadOnly|Function	0
	get				NESDBInstanceImp::E_get					DontDelete|DontEnum|ReadOnly|Function	1
	size			NESDBInstanceImp::E_size				DontDelete|DontEnum|ReadOnly|Function	0
@end
*/

const ClassInfo NESDBInstanceImp::info = {"NESDB", 0, 0, 0};

NESDBInstanceImp::NESDBInstanceImp(ObjectImp* proto)
  : ObjectImp(proto)
{
	NESDB_Init(&nesdb);
}

NESDBInstanceImp::~NESDBInstanceImp()
{
	NESDB_Close(&nesdb);
}

Value NESDBInstanceImp::get(ExecState *exec, const Identifier &propertyName) const
{
	return lookupGet<NESDBProtoFuncImp, NESDBInstanceImp, ObjectImp>(exec, propertyName, &nesdbTable, this);
}

Value NESDBInstanceImp::getValueProperty(ExecState *exec, int token) const
{
	return Undefined();
}

// PROTOTYPE

const ClassInfo NESDBPrototypeImp::info = {"NESDB", 0, &nesdbTable, 0};

NESDBPrototypeImp::NESDBPrototypeImp(ExecState*, ObjectPrototypeImp* objectProto)
  : NESDBInstanceImp(objectProto)
{
	Value protect(this);
	setInternalValue(Null());
}

Value NESDBPrototypeImp::get(ExecState* exec, const Identifier& propertyName) const
{
	return lookupGetFunction<NESDBProtoFuncImp, ObjectImp>(exec, propertyName, &nesdbTable, this);
}

NESDBProtoFuncImp::NESDBProtoFuncImp(ExecState* exec, int i, int len)
  : InternalFunctionImp(
    static_cast<FunctionPrototypeImp*>(exec->interpreter()->builtinFunctionPrototype().imp())
    ), id(abs(i))
{
	Value protect(this);
	putDirect(lengthPropertyName, len, DontDelete|ReadOnly|DontEnum);
}

enum { E_toString, E_open, E_close, E_get };

Value NESDBProtoFuncImp::call(ExecState* exec, Object& thisObj, const List &args)
{
	if (thisObj.className() != "NESDB")
	{
		Object err = Error::create(exec, TypeError);
		exec->setException(err);
		return err;
	}
	NESDBInstanceImp* nesdbImp = (NESDBInstanceImp*) thisObj.imp();
	switch (id)
	{
		case NESDBInstanceImp::E_toString:
			return String("NESDB object");
		case NESDBInstanceImp::E_open:
		{
			if (args.size() > 0)
				return Boolean(NESDB_Open(&nesdbImp->nesdb, args[0].toString(exec).cstring().c_str())==1);
		}
		case NESDBInstanceImp::E_close:
			return Boolean(NESDB_Close(&nesdbImp->nesdb)==1);
		case NESDBInstanceImp::E_get:
		{
			if (args.size() > 0)
			{
				struct NES_DB_ENTRY entry;
				NESDB_Get(&nesdbImp->nesdb, args[0].toUInt32(exec), &entry);
				
				Object tempobj = exec->interpreter()->builtinObject().construct(exec, List());
				tempobj.put(exec, Identifier("crc32"), Number(entry.crc32));
				tempobj.put(exec, Identifier("titleshot"), Number(entry.titleshot));
				tempobj.put(exec, Identifier("screenshot"), Number(entry.screenshot));
				tempobj.put(exec, Identifier("manual"), Number(entry.manual));
				tempobj.put(exec, Identifier("faqs"), Number(entry.faqs));
				tempobj.put(exec, Identifier("nsf"), Number(entry.nsf));
				tempobj.put(exec, Identifier("boxscan"), Number(entry.boxscan));
				tempobj.put(exec, Identifier("cartscan"), Number(entry.cartscan));
				tempobj.put(exec, Identifier("strategyguide"), Number(entry.strategyguide));
				tempobj.put(exec, Identifier("gamegenie"), Number(entry.gamegenie));
				tempobj.put(exec, Identifier("romnum"), Number(entry.romnum));
				tempobj.put(exec, Identifier("name"), String(entry.name));
				
				return tempobj;
			}
		}
		case NESDBInstanceImp::E_size:
		{
			return Number(nesdbImp->nesdb.recCount); 
		}
	}
	return Undefined();
}

//OBJECT

NESDBObjectImp::NESDBObjectImp(ExecState* exec, FunctionPrototypeImp* funcProto, NESDBPrototypeImp* nesdbProto)
  : InternalFunctionImp(funcProto)
{
  Value protect(this);
  
  putDirect(prototypePropertyName, nesdbProto, DontEnum|DontDelete|ReadOnly);

  // no. of arguments for constructor
  putDirect(lengthPropertyName, 0, ReadOnly|DontDelete|DontEnum);
}

Object NESDBObjectImp::construct(ExecState* exec, const List& args)
{
	return Object(new NESDBInstanceImp(g_NESDBPrototype.imp()));
}

