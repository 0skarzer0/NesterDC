#include "vmufile_object.h"
#include "../kjs/error_object.h"
#include "../kjs/operations.h"
#include "../kjs/lookup.h"

#include "vmufile_object.lut.h"

using namespace KJS;

Object g_VMUFilePrototype;
Object g_VMUFile;

void InitVMUFileClass(ExecState* exec, Object global)
{
	VMUFilePrototypeImp* vmufileProto = new VMUFilePrototypeImp(exec, (ObjectPrototypeImp*) exec->interpreter()->builtinObjectPrototype().imp());
	g_VMUFilePrototype = Object(vmufileProto);
	g_VMUFile = Object(new VMUFileObjectImp(exec, (FunctionPrototypeImp*) exec->interpreter()->builtinFunctionPrototype().imp(), vmufileProto));
	global.put(exec, "VMUFile", g_VMUFile, DontEnum|ReadOnly);
}

//INSTANCE

const ClassInfo VMUFileInstanceImp::info = {"VMUFile", 0, 0, 0};

void initVMUPkg(vmu_pkg_t* pPkg)
{
	pPkg->desc_short[0] = '\0';
	pPkg->desc_long[0] = '\0';
	pPkg->app_id[0] = '\0';
	pPkg->icon_cnt = 0;
	pPkg->icon_anim_speed = 0;
	pPkg->eyecatch_type = VMUPKG_EC_NONE;
	pPkg->data_len = 0;
	pPkg->icon_data = NULL;
	pPkg->eyecatch_data = NULL;
	pPkg->data = NULL;
}

VMUFileInstanceImp::VMUFileInstanceImp(ObjectImp* proto) : ObjectImp(proto)
{
	initVMUPkg(&pkg);
}

VMUFileInstanceImp::~VMUFileInstanceImp()
{
	if (pkg.data)
		free(pkg.data);
}

/*

enum { E_shortDesc, E_longDesc, appID, E_data };

@begin vmufileInstanceTable 5
	shortDesc		VMUFileInstanceImp::E_shortDesc		DontDelete	0
	longDesc		VMUFileInstanceImp::E_longDesc		DontDelete	0
	appID			VMUFileInstanceImp::E_appID			DontDelete	0
	data			VMUFileInstanceImp::E_data			DontDelete	0
@end
*/

Value VMUFileInstanceImp::get(ExecState *exec, const Identifier &propertyName) const
{
	return lookupGet<VMUFileProtoFuncImp, VMUFileInstanceImp, ObjectImp>(exec, propertyName, &vmufileInstanceTable, this);
}

Value VMUFileInstanceImp::getValueProperty(ExecState *exec, int token) const
{
	switch (token)
	{
		case E_shortDesc:
			return String(pkg.desc_short);
		case E_longDesc:
			return String(pkg.desc_long);
		case E_appID:
			return String(pkg.app_id);
		case E_data:
		{
			if (pkg.data)
				return String((char*)pkg.data);
			else
				return Null();
		}
	}
	return Undefined();
}

void VMUFileInstanceImp::put(ExecState *exec, const Identifier &propertyName, const Value &value, int attr)
{
	lookupPut<VMUFileInstanceImp, ObjectImp>(exec, propertyName, value, attr, &vmufileInstanceTable, this);
}

void VMUFileInstanceImp::putValueProperty(ExecState *exec, int token, const Value &value, int attr)
{
	switch (token)
	{
		case E_shortDesc:
		{
			const char* szShort = value.toString(exec).cstring().c_str();
			strncpy(pkg.desc_short, szShort, 19);
			break;
		}
		case E_longDesc:
		{
			const char* szLong = value.toString(exec).cstring().c_str();
			strncpy(pkg.desc_long, szLong, 35);
			break;
		}
		case E_appID:
		{
			const char* szAppID = value.toString(exec).cstring().c_str();
			strncpy(pkg.app_id, szAppID, 19);
			break;
		}
		case E_data:
		{
			const char* data = value.toString(exec).cstring().c_str();
			if (pkg.data)
				free(pkg.data);
			pkg.data = (uint8*) malloc(strlen(data)+1);
			strcpy((char*)pkg.data, data);
			pkg.data[strlen(data)] = 0;
			break;
		}
	}
}

CString VMUFileInstanceImp::toString()
{
	return "[VMUFile object]";
}

// PROTOTYPE

const ClassInfo VMUFilePrototypeImp::info = {"VMUFile", 0, &vmufileProtoTable, 0};

/*

enum { E_toString, E_save, E_load };

@begin vmufileProtoTable 4
	toString		VMUFileProtoFuncImp::E_toString		DontEnum|Function	0
	save			VMUFileProtoFuncImp::E_save			DontEnum|Function	1
	load			VMUFileProtoFuncImp::E_load			DontEnum|Function	1
@end
*/

VMUFilePrototypeImp::VMUFilePrototypeImp(ExecState*, ObjectPrototypeImp* objectProto)
  : VMUFileInstanceImp(objectProto)
{
	Value protect(this);
	setInternalValue(Null());
}

Value VMUFilePrototypeImp::get(ExecState* exec, const Identifier& propertyName) const
{
	return lookupGetFunction<VMUFileProtoFuncImp, ObjectImp>(exec, propertyName, &vmufileProtoTable, this);
}

VMUFileProtoFuncImp::VMUFileProtoFuncImp(ExecState* exec, int i, int len)
  : InternalFunctionImp(
    static_cast<FunctionPrototypeImp*>(exec->interpreter()->builtinFunctionPrototype().imp())
    ), id(abs(i))
{
	Value protect(this);
	putDirect(lengthPropertyName, len, DontDelete|ReadOnly|DontEnum);
}

Value VMUFileProtoFuncImp::call(ExecState* exec, Object& thisObj, const List &args)
{
	if (thisObj.className() != "VMUFile")
	{
		Object err = Error::create(exec, TypeError);
		exec->setException(err);
		return err;
	}
	VMUFileInstanceImp* f = (VMUFileInstanceImp*) thisObj.imp();
	switch (id)
	{
		case VMUFileProtoFuncImp::E_toString:
			return String(f->toString().c_str());
		case VMUFileProtoFuncImp::E_save:
		{
			if (!f->pkg.data)
				return Boolean(false);
			if (args.size() > 0)
			{
				CString filename = args[0].toString(exec).cstring();
				f->pkg.data_len = strlen((char*)f->pkg.data);
				
				uint8* newData;
				int newDataSize;
				
				if (vmu_pkg_build(&f->pkg, &newData, &newDataSize) < 0)
					return Boolean(false);
				
				FILE* fp = fopen(filename.c_str(), "wb");
				fwrite(newData, sizeof(uint8), newDataSize, fp);
				fclose(fp);
				free(newData);
				return Boolean(true);
			}
			break;
		}
		case VMUFileProtoFuncImp::E_load:
		{
			if (args.size() > 0)
			{
				CString filename = args[0].toString(exec).cstring();
				
				FILE* fp = fopen(filename.c_str(), "rb");
				if (!fp)
					return Boolean(false);
				fseek(fp, 0, SEEK_END);
				int rawFilesize = ftell(fp);
				fseek(fp, 0, SEEK_SET);
				uint8* rawData = (uint8*) malloc(rawFilesize+1);
				fread(rawData, sizeof(uint8), rawFilesize, fp);
				fclose(fp);
				rawData[rawFilesize] = 0;
				vmu_pkg_t tempPkg;
				if (vmu_pkg_parse(rawData, &tempPkg) < 0)
				{
					free(rawData);
					return Boolean(false);
				}
				if (f->pkg.data)
					free(f->pkg.data);
				initVMUPkg(&f->pkg);
				f->pkg.data = (uint8*) malloc(tempPkg.data_len+1);
				memcpy(f->pkg.data, tempPkg.data, tempPkg.data_len);
				f->pkg.data[tempPkg.data_len] = 0;
				strcpy(f->pkg.desc_short, tempPkg.desc_short);
				strcpy(f->pkg.desc_long, tempPkg.desc_long);
				strcpy(f->pkg.app_id, tempPkg.app_id);
				free(rawData);
				return Boolean(true);
			}
			break;
		}
		
	}
	return Undefined();
}

//OBJECT

VMUFileObjectImp::VMUFileObjectImp(ExecState* exec, FunctionPrototypeImp* funcProto, VMUFilePrototypeImp* vmufileProto)
  : InternalFunctionImp(funcProto)
{
  Value protect(this);
  
  putDirect(prototypePropertyName, vmufileProto, DontEnum|DontDelete|ReadOnly);

  // no. of arguments for constructor
  putDirect(lengthPropertyName, 0, ReadOnly|DontDelete|DontEnum);
}

Object VMUFileObjectImp::construct(ExecState* exec, const List& args)
{
	VMUFileInstanceImp* temp = new VMUFileInstanceImp(g_VMUFilePrototype.imp());
	return Object(temp);
}

