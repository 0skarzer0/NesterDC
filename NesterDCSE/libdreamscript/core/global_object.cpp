#include "global_object.h"
#include "../kjs/lookup.h"

#include "dc_utils.h"
#include "hashmap.h"
#include <imageload/imageload.h>

const ClassInfo GlobalImp::info = { "Global", 0, 0, 0 };

struct hashmap* includeOnceMap = NULL;

GlobalImp::GlobalImp() : ObjectImp()
{
	
}

GlobalFunctionImp::GlobalFunctionImp(ExecState* exec, int i, int len) : ObjectImp()
{
	id = i;
	put(exec, lengthPropertyName, Number(len), DontDelete|ReadOnly|DontEnum);
}

#define ARGB_PACK_COLOR(a, r, g, b) ( \
	( ((uint8)(a)) << 24 ) | \
	( ((uint8)(r)) << 16 ) | \
	( ((uint8)(g)) << 8 ) | \
	( ((uint8)(b)) << 0 ) )

Value GlobalFunctionImp::call(ExecState *exec, Object &thisObj, const List &args)
{
	switch (id)
	{
		case GlobalFunctionImp::E_toString:
			return String("Global Object");
			break;
		case GlobalFunctionImp::E_print:
		{
			for (int k = 0; k < args.size(); ++k)
				dbgio_printf(args[k].toString(exec).cstring().c_str());
			break;
		}
		case GlobalFunctionImp::E_println:
		{
			for (int k = 0; k < args.size(); ++k)
				dbgio_printf(args[k].toString(exec).cstring().c_str());
			dbgio_printf("\n");
			break;
		}
		case GlobalFunctionImp::E_include:
		case GlobalFunctionImp::E_includeOnce:
		{
			CString scriptfile = args[0].toString(exec).cstring();
			bool hasit = hashmap_get(includeOnceMap, scriptfile.c_str()) != NULL;
			if (id == GlobalFunctionImp::E_includeOnce && hasit)
				return Boolean((bool)false);
			char* sztemp = read_text_file(scriptfile.c_str());
			if (sztemp)
			{
				if (!hasit)
					hashmap_put(includeOnceMap, (void*) scriptfile.c_str(), (void*) 111111);
				Interpreter* interp = exec->interpreter();
				Completion comp(interp->evaluate(sztemp, thisObj, scriptfile));
				delete[] sztemp;
				return Boolean((bool)true);
			}
			return Boolean((bool)false);
			break;
		}
		case GlobalFunctionImp::E_packColor:
		{
			if (args.size() > 3)
				return Number(ARGB_PACK_COLOR(args[0].toUInt32(exec), args[1].toUInt32(exec), args[2].toUInt32(exec), args[3].toUInt32(exec)));
			break;
		}
	}
	return Undefined();
}

void InitGlobal(ExecState* exec, Object obj)
{
	includeOnceMap = hashmap_new(hash_str, cmp_str, NULL, stdlib_allocator);
	
	obj.put(exec, Identifier("toString"), Object(new GlobalFunctionImp(exec, GlobalFunctionImp::E_toString, 0)), ReadOnly|DontEnum|Function);
	obj.put(exec, Identifier("print"), Object(new GlobalFunctionImp(exec, GlobalFunctionImp::E_print, 1)), ReadOnly|DontEnum|Function);
	obj.put(exec, Identifier("println"), Object(new GlobalFunctionImp(exec, GlobalFunctionImp::E_println, 1)), ReadOnly|DontEnum|Function);
	obj.put(exec, Identifier("include"), Object(new GlobalFunctionImp(exec, GlobalFunctionImp::E_include, 1)), ReadOnly|DontEnum|Function);
	obj.put(exec, Identifier("includeOnce"), Object(new GlobalFunctionImp(exec, GlobalFunctionImp::E_includeOnce, 1)), ReadOnly|DontEnum|Function);
	obj.put(exec, Identifier("packColor"), Object(new GlobalFunctionImp(exec, GlobalFunctionImp::E_packColor, 1)), ReadOnly|DontEnum|Function);
	
	obj.put(exec, Identifier("IMG_ALPHA_NONE"), Number(IMG_ALPHA_NONE), ReadOnly|DontEnum);
	obj.put(exec, Identifier("IMG_ALPHA_MASK"), Number(IMG_ALPHA_MASK), ReadOnly|DontEnum);
	obj.put(exec, Identifier("IMG_ALPHA_KEYED"), Number(IMG_ALPHA_KEYED), ReadOnly|DontEnum);
	obj.put(exec, Identifier("IMG_ALPHA_FULL"), Number(IMG_ALPHA_FULL), ReadOnly|DontEnum);
	
	obj.put(exec, Identifier("IMG_OPAQUE"), Number(PVR_LIST_OP_POLY), ReadOnly|DontEnum);
	obj.put(exec, Identifier("IMG_TRANSPARENT"), Number(PVR_LIST_TR_POLY), ReadOnly|DontEnum);
	obj.put(exec, Identifier("IMG_PUNCH_THRU"), Number(PVR_LIST_PT_POLY), ReadOnly|DontEnum);
	
	obj.put(exec, Identifier("IMG_FILTER_NONE"), Number(PVR_FILTER_NONE), ReadOnly|DontEnum);
	obj.put(exec, Identifier("IMG_FILTER_NEAREST"), Number(PVR_FILTER_NEAREST), ReadOnly|DontEnum);
	obj.put(exec, Identifier("IMG_FILTER_BILINEAR"), Number(PVR_FILTER_BILINEAR), ReadOnly|DontEnum);
	obj.put(exec, Identifier("IMG_FILTER_TRILINEAR1"), Number(PVR_FILTER_TRILINEAR1), ReadOnly|DontEnum);
	obj.put(exec, Identifier("IMG_FILTER_TRILINEAR2"), Number(PVR_FILTER_TRILINEAR2), ReadOnly|DontEnum);
}
