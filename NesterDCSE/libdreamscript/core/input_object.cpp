#include "input_object.h"
#include "../kjs/lookup.h"
#include "input_object.lut.h"

#include "dc_utils.h"

const ClassInfo InputImp::info = { "Input", 0, &inputTable, 0 };

InputImp* g_inputImp = NULL;
XML* g_inputXML = NULL;

/* Source for input_object.lut.h

enum { E_toString, E_scanDevices };

@begin inputTable 3
	toString			InputImp::E_toString			DontDelete|ReadOnly|Function	0
	scanDevices			InputImp::E_scanDevices			DontDelete|ReadOnly|Function	0
@end
*/

InputImp::InputImp() : ObjectImp()
{
	
}

void InputImp::rescanDevices()
{
	
}

Value InputImp::get(ExecState *exec, const Identifier &propertyName) const
{
	return lookupGetFunction<InputFunctionImp, ObjectImp>(exec, propertyName, &inputTable, this);
}
/*
Value InputImp::getValueProperty(ExecState *exec, int token) const
{
	return Undefined();
}
*/
InputFunctionImp::InputFunctionImp(ExecState* exec, int i, int len) : ObjectImp()
{
	id = i;
	put(exec, lengthPropertyName, Number(len), DontDelete|ReadOnly|DontEnum);
}

Value InputFunctionImp::call(ExecState *exec, Object &thisObj, const List &args)
{
	switch (id)
	{
		case InputImp::E_toString:
		{
			return String("Input Object");
			break;
		}
		case InputImp::E_scanDevices:
		{
			dc_maple_init();
			break;
		}
	}
	return Undefined();
}

void InitDefaultMappings()
{
	
}

void InitInputObject(ExecState* exec, Object obj)
{
	dc_maple_init();
	g_inputImp = new InputImp();
	obj.put(exec, Identifier("Input"), Object(g_inputImp));
	
	g_inputXML = XML_OpenFile("/cd/dscript.cfg");
	if (!g_inputXML)
	{
		printf("default mappings\n");
		InitDefaultMappings();
	}
}
