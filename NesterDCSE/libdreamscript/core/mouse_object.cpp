#include "mouse_object.h"
#include "dc_utils.h"

#include "../kjs/lookup.h"

#include "mouse_object.lut.h"

const ClassInfo MouseImp::info = { "Mouse", 0, &mouseTable, 0 };

/* Source for mouse_object.lut.h

enum { E_poll, E_dx, E_dy, E_dz, E_lbutton, E_rbutton, E_sbutton };

@begin mouseTable 8
	poll		MouseImp::E_poll		DontDelete|ReadOnly|Function	0
	dx			MouseImp::E_dx			DontDelete|ReadOnly
	dy			MouseImp::E_dy			DontDelete|ReadOnly
	dz			MouseImp::E_dz			DontDelete|ReadOnly
	lbutton		MouseImp::E_lbutton		DontDelete|ReadOnly
	rbutton		MouseImp::E_rbutton		DontDelete|ReadOnly
	sbutton		MouseImp::E_sbutton		DontDelete|ReadOnly
@end
*/

MouseImp::MouseImp(struct DCMouse* mouse) : ObjectImp()
{
	m_mouse = mouse;
}

Value MouseImp::get(ExecState *exec, const Identifier &propertyName) const
{
	return lookupGet<MouseFunctionImp, MouseImp, ObjectImp>(exec, propertyName, &mouseTable, this);
}

Value MouseImp::getValueProperty(ExecState *exec, int token) const
{
	switch (token)
	{
		case MouseImp::E_dx:
			return Number(m_mouse->state.dx);
		case MouseImp::E_dy:
			return Number(m_mouse->state.dy);
		case MouseImp::E_dz:
			return Number(m_mouse->state.dz);
		case MouseImp::E_lbutton:
			return Boolean(DCMouse_IsPressed(m_mouse, MOUSE_LEFTBUTTON));
		case MouseImp::E_rbutton:
			return Boolean(DCMouse_IsPressed(m_mouse, MOUSE_RIGHTBUTTON));
		case MouseImp::E_sbutton:
			return Boolean(DCMouse_IsPressed(m_mouse, MOUSE_SIDEBUTTON));
	}
	return Undefined();
}

MouseFunctionImp::MouseFunctionImp(ExecState* exec, int i, int len) : ObjectImp()
{
	id = i;
	put(exec, lengthPropertyName, Number(len), DontDelete|ReadOnly|DontEnum);
}

Value MouseFunctionImp::call(ExecState *exec, Object &thisObj, const List &args)
{
	if (thisObj.className() != "Mouse")
	{
		Object err = Error::create(exec, TypeError);
		exec->setException(err);
		return err;
	}
	switch (id)
	{
		case MouseImp::E_poll:
			MouseImp* mouseImp = (MouseImp*) thisObj.imp();
			DCMouse_Poll(mouseImp->m_mouse);
			break;
	}
	return Undefined();
}

void InitMice(ExecState* exec, Object obj)
{
	List locargs;
	int k;
	for (k = 0; k < 4; ++k)
	{
		DCMouse_Init(g_mice + k);
		g_mice[k].dev = dc_maple_mouse_info[k].dev;
		if (g_mice[k].dev)
		{
			Object tempobj(new MouseImp(g_mice + k));
			locargs.append(tempobj);
		}
	}
	Object pad = exec->interpreter()->builtinArray().construct(exec, locargs);
	
	//Object inputObj = obj.get(exec, Identifier("Input")).toObject(exec);
	obj.put(exec, Identifier("mouse"), pad);
}
