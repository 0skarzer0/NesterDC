#include "controller_object.h"
#include "dc_utils.h"
#include "allocator.h"
#include "../kjs/lookup.h"
#include "input_object.h"

#define POLL_FUNC			(CONT_DPAD2_RIGHT+1)
#define CONT_LTRIG			(CONT_DPAD2_RIGHT+2)
#define CONT_RTRIG			(CONT_DPAD2_RIGHT+3)
#define CONT_XAXIS			(CONT_DPAD2_RIGHT+4)
#define CONT_YAXIS			(CONT_DPAD2_RIGHT+5)
#define SETMODE_FUNC		(CONT_DPAD2_RIGHT+6)
#define GETMODE_FUNC		(CONT_DPAD2_RIGHT+7)
#define JUSTPRESSED_FUNC	(CONT_DPAD2_RIGHT+8)

const ClassInfo ControllerImp::info = { "Controller", 0, 0, 0 };
struct hashmap* ControllerImp::storedMaps = NULL;

ControllerImp::ControllerImp(struct DCController* pad) : ObjectImp()
{
	m_pad = pad;
	cmdMap = (struct hashmap*) hashmap_get(storedMaps, "default");
}

struct hashmap* CreateBarebonesMap()
{
	struct hashmap* tempmap = hashmap_new(hash_str, cmp_str, NULL, stdlib_allocator);
	hashmap_put(tempmap, (void*) "poll",			(void*) POLL_FUNC);
	hashmap_put(tempmap, (void*) "setMode",			(void*) SETMODE_FUNC);
	hashmap_put(tempmap, (void*) "getMode",			(void*) GETMODE_FUNC);
	hashmap_put(tempmap, (void*) "justPressed",		(void*) JUSTPRESSED_FUNC);
	return tempmap;
}

bool ControllerImp::InitDefaultMappings()
{
	struct hashmap* tempmap = CreateBarebonesMap();
	storedMaps = hashmap_new(hash_str, cmp_str, NULL, stdlib_allocator);

	// default mappings
#ifdef _arch_dreamcast
	hashmap_put(tempmap, (void*) "A",				(void*) CONT_A);
	hashmap_put(tempmap, (void*) "B",				(void*) CONT_B);
	hashmap_put(tempmap, (void*) "X",				(void*) CONT_X);
	hashmap_put(tempmap, (void*) "Y",				(void*) CONT_Y);
	hashmap_put(tempmap, (void*) "START",			(void*) CONT_START);
	hashmap_put(tempmap, (void*) "UP",				(void*) CONT_DPAD_UP);
	hashmap_put(tempmap, (void*) "DOWN",			(void*) CONT_DPAD_DOWN);
	hashmap_put(tempmap, (void*) "LEFT",			(void*) CONT_DPAD_LEFT);
	hashmap_put(tempmap, (void*) "RIGHT",			(void*) CONT_DPAD_RIGHT);
	hashmap_put(tempmap, (void*) "LTRIG",			(void*) CONT_LTRIG);
	hashmap_put(tempmap, (void*) "RTRIG",			(void*) CONT_RTRIG);
	hashmap_put(tempmap, (void*) "XAXIS",			(void*) CONT_XAXIS);
	hashmap_put(tempmap, (void*) "YAXIS",			(void*) CONT_YAXIS);

#else
	#error You must define an architecture. Choices are: _arch_dreamcast
#endif
	hashmap_put(storedMaps, (void*) "default",	tempmap);
	return true;
}

bool ControllerImp::LoadMappingsXML(XML* xmlnode)
{
	XML* systemnode = XML_Find(xmlnode, "system", "name", "dreamcast");
	if (!systemnode)
		return false;
	XML* inputnode = XML_Find(xmlnode, "input", NULL, NULL);
	if (!inputnode)
		return false;
	uint32 k;
	XML* modenode;
	XML* controllernode;
	const char* strTempName;
	for (k = 0; k < inputnode->children.count; ++k)
	{
		modenode = inputnode->children.data[k];
		if (stricmp(modenode->name, "mode") != 0)
			continue;
		strTempName = XML_GetAttribute(modenode, "name");
		if (!strTempName)
			continue;
		controllernode = XML_Find(modenode, "controller", NULL, NULL);
		if (!controllernode)
			continue;
		uint32 n = 0;
		struct hashmap* tempMap = CreateBarebonesMap();
		const char* strButtonName;
		const char* strButtonMapping;
		XML* actionnode;
		uint32 iButton;
		for (; n < controllernode->children.count; ++n)
		{
			actionnode = controllernode->children.data[n];
			strButtonName = XML_GetAttribute(actionnode, "name");
			strButtonMapping = XML_GetAttribute(actionnode, "mapping");
			if (stricmp(actionnode->name, "action") != 0 || !strButtonName || !strButtonMapping)
				continue;
			if (stricmp(strButtonMapping, "A") == 0)
				iButton = CONT_A;
			else if (stricmp(strButtonMapping, "B") == 0)
				iButton = CONT_B;
			else if (stricmp(strButtonMapping, "X") == 0)
				iButton = CONT_X;
			else if (stricmp(strButtonMapping, "Y") == 0)
				iButton = CONT_Y;
			else if (stricmp(strButtonMapping, "START") == 0)
				iButton = CONT_START;
			else if (stricmp(strButtonMapping, "UP") == 0)
				iButton = CONT_DPAD_UP;
			else if (stricmp(strButtonMapping, "DOWN") == 0)
				iButton = CONT_DPAD_DOWN;
			else if (stricmp(strButtonMapping, "LEFT") == 0)
				iButton = CONT_DPAD_LEFT;
			else if (stricmp(strButtonMapping, "RIGHT") == 0)
				iButton = CONT_DPAD_RIGHT;
			else if (stricmp(strButtonMapping, "XAXIS") == 0)
				iButton = CONT_XAXIS;
			else if (stricmp(strButtonMapping, "YAXIS") == 0)
				iButton = CONT_YAXIS;
			else if (stricmp(strButtonMapping, "LTRIG") == 0)
				iButton = CONT_LTRIG;
			else if (stricmp(strButtonMapping, "RTRIG") == 0)
				iButton = CONT_RTRIG;
			else
				continue;
			hashmap_put(tempMap, (void*) strButtonName, (void*) iButton);
		}
		hashmap_put(storedMaps, (void*) strTempName, tempMap);
	}
	return true;
}

Value ControllerImp::get(ExecState *exec, const Identifier &propertyName) const
{
	uint32 val = (uint32) hashmap_get(cmdMap, propertyName.ascii());
	switch (val)
	{
		case POLL_FUNC:
			return lookupOrCreateFunction<ControllerFunctionImp>(exec, propertyName, this, ControllerImp::e_poll, 0, DontDelete|ReadOnly|Function);
		case SETMODE_FUNC:
			return lookupOrCreateFunction<ControllerFunctionImp>(exec, propertyName, this, ControllerImp::e_setMode, 1, DontDelete|ReadOnly|Function);
		case GETMODE_FUNC:
			return lookupOrCreateFunction<ControllerFunctionImp>(exec, propertyName, this, ControllerImp::e_getMode, 0, DontDelete|ReadOnly|Function);
		case JUSTPRESSED_FUNC:
			return lookupOrCreateFunction<ControllerFunctionImp>(exec, propertyName, this, ControllerImp::e_justPressed, 1, DontDelete|ReadOnly|Function);
		case CONT_LTRIG:
			return Number(m_pad->state.ltrig);
		case CONT_RTRIG:
			return Number(m_pad->state.rtrig);
		case CONT_XAXIS:
			return Number(m_pad->state.joyx);
		case CONT_YAXIS:
			return Number(m_pad->state.joyy);
		case CONT_A:
		case CONT_B:
		case CONT_X:
		case CONT_Y:
		case CONT_START:
		case CONT_DPAD_UP:
		case CONT_DPAD_DOWN:
		case CONT_DPAD_LEFT:
		case CONT_DPAD_RIGHT:
			return Boolean(DCController_IsPressed(m_pad, val));
			break;
	}
	return ObjectImp::get(exec, propertyName);
}

void ControllerImp::poll()
{
	DCController_Poll(m_pad);
}

bool ControllerImp::setMode(const char* strMapName)
{
	struct hashmap* tempmap = (struct hashmap*) hashmap_get(storedMaps, strMapName);
	if (!tempmap)
		return false;
	cmdMap = tempmap;
	modeName = strMapName;
	return true;
}

CString ControllerImp::getMode()
{
	return modeName;
}

bool ControllerImp::justPressed(const char* strval)
{
	uint32 val = (uint32) hashmap_get(cmdMap, strval);
	switch (val)
	{
		case CONT_LTRIG:
			return (!m_pad->old_state.ltrig && m_pad->state.ltrig);
		case CONT_RTRIG:
			return (!m_pad->old_state.rtrig && m_pad->state.rtrig);
		case CONT_A:
		case CONT_B:
		case CONT_X:
		case CONT_Y:
		case CONT_START:
		case CONT_DPAD_UP:
		case CONT_DPAD_DOWN:
		case CONT_DPAD_LEFT:
		case CONT_DPAD_RIGHT:
			return DCController_JustPressed(m_pad, val);
			break;
	}
	return false;
}

ControllerFunctionImp::ControllerFunctionImp(ExecState* exec, int i, int len) : ObjectImp()
{
	id = i;
	put(exec, lengthPropertyName, Number(len), DontDelete|ReadOnly|DontEnum);
}

Value ControllerFunctionImp::call(ExecState *exec, Object &thisObj, const List &args)
{
	if (thisObj.className() != "Controller")
	{
		Object err = Error::create(exec, TypeError);
		exec->setException(err);
		return err;
	}
	switch (id)
	{
		case ControllerImp::e_poll:
		{
			((ControllerImp*) thisObj.imp())->poll();
			break;
		}
		case ControllerImp::e_setMode:
		{
			if (args.size() == 1)
			{
				CString str = args[0].toString(exec).cstring();
				return Boolean(((ControllerImp*) thisObj.imp())->setMode(str.c_str()));
			}
			break;
		}
		case ControllerImp::e_getMode:
		{
			return String(((ControllerImp*) thisObj.imp())->getMode().c_str());
			break;
		}
		case ControllerImp::e_justPressed:
		{
			if (args.size() == 1)
			{
				CString str = args[0].toString(exec).cstring();
				return Boolean(((ControllerImp*) thisObj.imp())->justPressed(str.c_str()));
			}
			break;
		}
	}
	return Undefined();
}

void InitControllers(ExecState* exec, Object obj)
{
	ControllerImp::InitDefaultMappings();
	if (g_inputXML)
		ControllerImp::LoadMappingsXML(g_inputXML);
	else
		dbgio_printf("g_inputXML == NULL\n");
	List locargs;
	int k;
	ControllerImp* tempControllerImp;
	for (k = 0; k < 4; ++k)
	{
		DCController_Init(g_controllers + k);
		g_controllers[k].dev = dc_maple_controller_info[k].dev;
		if (g_controllers[k].dev)
		{
			Object tempobj(tempControllerImp = new ControllerImp(g_controllers + k));
			locargs.append(tempobj);
		}
	}
	Object pad = exec->interpreter()->builtinArray().construct(exec, locargs);
	obj.put(exec, Identifier("controller"), pad);
}
