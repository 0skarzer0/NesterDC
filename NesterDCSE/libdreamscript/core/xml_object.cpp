#include "xml_object.h"
#include "../kjs/lookup.h"
#include "xml_object.lut.h"

#include "dc_utils.h"
#include <imageload/imageload.h>
#include "music_object.h"

const ClassInfo XMLImp::info = { "XML", 0, &xmlTable, 0 };

XMLImp* g_xmlImp = NULL;

/* Source for xml_object.lut.h

enum { E_toString, E_file2Obj, E_str2Obj };

@begin xmlTable 3
	toString		XMLImp::E_toString		DontDelete|ReadOnly|Function	0
	file2Obj		XMLImp::E_file2Obj		DontDelete|ReadOnly|Function	1
	str2Obj			XMLImp::E_str2Obj		DontDelete|ReadOnly|Function	1
@end
*/

XMLImp::XMLImp() : ObjectImp()
{
	
}

Value XMLImp::get(ExecState *exec, const Identifier &propertyName) const
{
	return lookupGet<XMLFunctionImp, XMLImp, ObjectImp>(exec, propertyName, &xmlTable, this);
}

Value XMLImp::getValueProperty(ExecState *exec, int token) const
{
	switch (token)
	{
		//case XMLImp::E_x:
		//	return Number(screen_adjustments.x1);
	};
	return Undefined();
}

void XMLImp::put(ExecState *exec, const Identifier &propertyName, const Value &value, int attr)
{
	lookupPut<XMLImp, ObjectImp>(exec, propertyName, value, attr, &xmlTable, this);
}

void XMLImp::putValueProperty(ExecState *exec, int token, const Value &value, int attr)
{
	switch (token)
	{
		/*
		case XMLImp::E_x:
			tempwidth = screen_adjustments.x2 - screen_adjustments.x1;
			screen_adjustments.x1 = value.toInteger(exec);
			screen_adjustments.x2 = screen_adjustments.x1 + tempwidth;
			break;
		*/
	};
}

XMLFunctionImp::XMLFunctionImp(ExecState* exec, int i, int len) : ObjectImp()
{
	id = i;
	put(exec, lengthPropertyName, Number(len), DontDelete|ReadOnly|DontEnum);
}

Object XML2JSObj(XML* rootxml, ExecState* exec)
{
	Object obj = exec->interpreter()->builtinObject().construct(exec, List());
	obj.put(exec, Identifier("_name"), String(rootxml->name));
	if (rootxml->text)
		obj.put(exec, Identifier("_text"), String(rootxml->text));
	else
		obj.put(exec, Identifier("_text"), String(""));
	uint32 k = 0;
	for (; k < rootxml->attribute_names.count; ++k)
		obj.put(exec, Identifier(rootxml->attribute_names.data[k]), String(rootxml->attribute_values.data[k]));
	List locargs;
	for (k = 0; k < rootxml->children.count; ++k)
		locargs.append(XML2JSObj(rootxml->children.data[k], exec));
	obj.put(exec, Identifier("_children"), Object(exec->interpreter()->builtinArray().construct(exec, locargs)));
	return obj;
}

Value XMLFunctionImp::call(ExecState *exec, Object &thisObj, const List &args)
{
	//XMLImp* imp = (XMLImp*) thisObj.imp();
	if (thisObj.className() != "XML")
	{
		Object err = Error::create(exec, TypeError);
		exec->setException(err);
		return err;
	}
	switch (id)
	{
		case XMLImp::E_toString:
		{
			return String("XML Object");
			break;
		}
		case XMLImp::E_file2Obj:
		{
			if (args.size() > 0)
			{
				CString filename = args[0].toString(exec).cstring();
				XML* rootxml = XML_OpenFile(filename.c_str());
				if (rootxml)
				{
					Object obj = XML2JSObj(rootxml, exec);
					XML_Destroy(rootxml);
					return obj;
				}
			}
			return Null();
			break;
		}
		case XMLImp::E_str2Obj:
		{
			if (args.size() > 0)
			{
				CString str = args[0].toString(exec).cstring();
				XML* rootxml = XML_Parse(str.c_str());
				if (rootxml)
				{
					Object obj = XML2JSObj(rootxml, exec);
					XML_Destroy(rootxml);
					return obj;
				}
			}
			return Null();
			break;
		}
	}
	return Undefined();
}

void InitXMLObject(ExecState* exec, Object obj)
{
	g_xmlImp = new XMLImp();
	obj.put(exec, Identifier("XML"), Object(g_xmlImp));
}
