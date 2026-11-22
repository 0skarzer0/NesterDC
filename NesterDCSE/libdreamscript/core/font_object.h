#ifndef _FONT_OBJECT_H_
#define _FONT_OBJECT_H_

#include "../kjs/value.h"
#include "../kjs/internal.h"
#include "../kjs/function_object.h"
#include "image.h"
#include "video_object.h"
#include "hashmap.h"

#include "../kjs/port_kjs.h"

using namespace KJS;

class FontInstanceImp : public ObjectImp, public DrawTarget
{
public:
	FontInstanceImp(ObjectImp* proto);
	virtual ~FontInstanceImp();

	enum { E_toString, E_load, E_unload, E_draw, E_drawWithBorder, E_getWidth, E_setBilinear };

	virtual const ClassInfo* classInfo() const { return &info; }
	virtual Value get(ExecState *exec, const Identifier &propertyName) const;
    Value getValueProperty(ExecState *exec, int token) const;
	//void put(ExecState *exec, const Identifier &propertyName, const Value &value, int attr = None);
    //void putValueProperty(ExecState *exec, int token, const Value &value, int attr = None);
	
	// Start JS Functions
	CString toString();
	
	bool load(const char* filename, float fixed_width);
	void unload();
	bool draw(const char* str, float x, float y, float z, float maxWidth, float size, uint32 color, bool reallyDraw = true);
	bool drawWithBorder(const char* str, float x, float y, float z, float maxWidth, float size, uint32 color, uint32 borderColor, bool reallyDraw = true);
	float getWidth(const char* str, float size);
	
	// End JS Functions
	
	//DrawTarget
	virtual void DoDraw(struct SDrawCommand& sCmd);
	
	plx_font_t* getPlxFont() { return fnt; }
	
	static const ClassInfo info;
	
protected:
	plx_font_t* fnt;
	plx_fcxt_t* cxt;
};

class FontPrototypeImp : public FontInstanceImp
{
public:
	FontPrototypeImp(ExecState* exec, ObjectPrototypeImp* objectProto);
	Value get(ExecState *exec, const Identifier &p) const;
	virtual const ClassInfo* classInfo() const { return &info; }
	static const ClassInfo info;
};

class FontProtoFuncImp : public InternalFunctionImp
{
public:
	FontProtoFuncImp(ExecState* exec, int i, int len);

	virtual bool implementsCall() const { return true; }
	virtual Value call(ExecState* exec, Object& thisObj, const List& args);
private:
	int id;
};

class FontObjectImp : public InternalFunctionImp
{
public:
	FontObjectImp(ExecState* exec, FunctionPrototypeImp* funcProto, FontPrototypeImp* dateProto);

	virtual bool implementsConstruct() const { return true; }
	virtual Object construct(ExecState* exec, const List &args);

	Object construct(const List &);
};

void InitFontClass(ExecState* exec, Object globobj);

#endif
