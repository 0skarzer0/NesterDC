#ifndef _MOUSE_OBJECT_H_
#define _MOUSE_OBJECT_H_

#include "../kjs/value.h"
#include "../kjs/object.h"
#include "../kjs/types.h"
#include "../kjs/interpreter.h"
#include "dc_mouse.h"

using namespace KJS;

class MouseFunctionImp;

class MouseImp : public ObjectImp
{
public:
	MouseImp(struct DCMouse* mouse);
	
	enum { E_poll, E_dx, E_dy, E_dz, E_lbutton, E_rbutton, E_sbutton };

	virtual UString className() const { return "Mouse"; }
	virtual Value get(ExecState *exec, const Identifier &propertyName) const;
    Value getValueProperty(ExecState *exec, int token) const;
	
	struct DCMouse* m_mouse;
	static const ClassInfo info;
};

class MouseFunctionImp : public ObjectImp
{
public:
	MouseFunctionImp(ExecState *exec, int i, int len);
	
	virtual bool implementsCall() const { return true; }
	virtual Value call(ExecState* exec, Object &thisObj, const List &args);
protected:
	uint32 id;
};

void InitMice(ExecState* exec, Object obj);

#endif
