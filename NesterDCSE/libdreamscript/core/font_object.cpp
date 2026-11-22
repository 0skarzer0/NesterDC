#include "font_object.h"
#include "../kjs/error_object.h"
#include "../kjs/operations.h"
#include "../kjs/lookup.h"
#include "image_object.h"
#include "XML.h"
#include "makesplitpath.h"
#include "punk_array.h"

#include "font_object.lut.h"

extern "C" {
#include <imageload/imageload.h>
}

using namespace KJS;

Object g_FontPrototype;
Object g_Font;

void InitFontClass(ExecState* exec, Object global)
{
	FontPrototypeImp* fontProto = new FontPrototypeImp(exec, (ObjectPrototypeImp*) exec->interpreter()->builtinObjectPrototype().imp());
	g_FontPrototype = Object(fontProto);
	g_Font = Object(new FontObjectImp(exec, (FunctionPrototypeImp*) exec->interpreter()->builtinFunctionPrototype().imp(), fontProto));
	global.put(exec, "Font", g_Font, DontEnum|ReadOnly);
}

//INSTANCE

/*

enum { E_toString, E_load, E_unload, E_draw, E_drawWithBorder, E_getWidth, E_setBilinear };

@begin fontTable 7
	toString		FontInstanceImp::E_toString			DontDelete|DontEnum|ReadOnly|Function	0
	load			FontInstanceImp::E_load				DontDelete|DontEnum|ReadOnly|Function	1
	unload			FontInstanceImp::E_unload			DontDelete|DontEnum|ReadOnly|Function	0
	draw			FontInstanceImp::E_draw				DontDelete|DontEnum|ReadOnly|Function	6
	drawWithBorder	FontInstanceImp::E_drawWithBorder	DontDelete|DontEnum|ReadOnly|Function	7
	getWidth		FontInstanceImp::E_getWidth			DontDelete|DontEnum|ReadOnly|Function	2
	setBilinear		FontInstanceImp::E_setBilinear		DontDelete|DontEnum|ReadOnly|Function	1
@end
*/

const ClassInfo FontInstanceImp::info = {"Font", 0, 0, 0};

FontInstanceImp::FontInstanceImp(ObjectImp* proto)
  : ObjectImp(proto)
{
	fnt = NULL;
	cxt = NULL;
}

FontInstanceImp::~FontInstanceImp()
{
	
}

Value FontInstanceImp::get(ExecState *exec, const Identifier &propertyName) const
{
	return lookupGet<FontProtoFuncImp, FontInstanceImp, ObjectImp>(exec, propertyName, &fontTable, this);
}

Value FontInstanceImp::getValueProperty(ExecState *exec, int token) const
{
	/*
	switch (token)
	{
		case FontInstanceImp::E_animation:
			return String(animation.c_str());
	}
	*/
	return Undefined();
}

CString FontInstanceImp::toString()
{
	return "Font object ";
}

bool FontInstanceImp::load(const char* filename, float fixed_width)
{
	fnt = plx_font_load(filename);
	cxt = plx_fcxt_create(fnt, PVR_LIST_TR_POLY);
	cxt->flags = 0;
	if (fixed_width > 0)
	{
		cxt->flags |= PLX_FCXT_FIXED;
		cxt->fixed_width = fixed_width;
	}
	return true;
}

void FontInstanceImp::unload()
{
	if (cxt)
		plx_fcxt_destroy(cxt);
	if (fnt)
		plx_font_destroy(fnt);
}

#define FONT_DRAW				0
#define FONT_DRAW_WITH_BORDER	1

class FontDrawInfo
{
public:
	uint32 type;
	char* str;
	float x, y, z, size, maxWidth;
	uint32 color, borderColor;
	bool didntCrop;
};

bool FontInstanceImp::draw(const char* str, float x, float y, float z, float maxWidth, float size, uint32 color, bool reallyDraw)
{
	if (strstr(str, "[TAB]"))
		maxWidth = -1;
	FontDrawInfo* drawinfo = new FontDrawInfo();
	drawinfo->type = FONT_DRAW;
	drawinfo->str = strdup(str);
	drawinfo->x = x;
	drawinfo->y = y;
	drawinfo->z = z;
	drawinfo->maxWidth = maxWidth;
	drawinfo->size = size;
	drawinfo->color = color;
	
	cxt->size = size;
	drawinfo->didntCrop = true;
	if (maxWidth > 0)
	{
		float outleft, outup, outright, outdown;
		int32 curnull = strlen(drawinfo->str);
		while (true)
		{
			plx_fcxt_str_metrics(cxt, drawinfo->str, &outleft, &outup, &outright, &outdown);
			if (outright > maxWidth)
			{
				drawinfo->didntCrop = false;
				--curnull;
				if (!curnull)
				{
					free(drawinfo->str);
					delete drawinfo;
					return false;
				}
				drawinfo->str[curnull] = '\0';
			}
			else
				break;
		}
	}
	if (!reallyDraw)
	{
		free(drawinfo->str);
		delete drawinfo;
		return drawinfo->didntCrop;
	}
	
	struct SDrawCommand sCmd;
	sCmd.pTarget = this;
	sCmd.pData = (void*) drawinfo;
	sCmd.list = PVR_LIST_TR_POLY;
	g_videoImp->AddDrawCommand(sCmd, false);
	return drawinfo->didntCrop;
}

bool FontInstanceImp::drawWithBorder(const char* str, float x, float y, float z, float maxWidth, float size, uint32 color, uint32 borderColor, bool reallyDraw)
{
	if (strstr(str, "[TAB]"))
		maxWidth = -1;
	FontDrawInfo* drawinfo = new FontDrawInfo();
	drawinfo->type = FONT_DRAW_WITH_BORDER;
	drawinfo->str = strdup(str);
	drawinfo->x = x;
	drawinfo->y = y;
	drawinfo->z = z;
	drawinfo->maxWidth = maxWidth;
	drawinfo->size = size;
	drawinfo->color = color;
	drawinfo->borderColor = borderColor;
	
	cxt->size = size;
	drawinfo->didntCrop = true;
	if (maxWidth > 0)
	{
		float outleft, outup, outright, outdown;
		int32 curnull = strlen(drawinfo->str);
		while (true)
		{
			plx_fcxt_str_metrics(cxt, drawinfo->str, &outleft, &outup, &outright, &outdown);
			if (outright > maxWidth)
			{
				drawinfo->didntCrop = false;
				--curnull;
				if (!curnull)
				{
					free(drawinfo->str);
					delete drawinfo;
					return false;
				}
				drawinfo->str[curnull] = '\0';
			}
			else
				break;
		}
	}
	if (!reallyDraw)
	{
		free(drawinfo->str);
		delete drawinfo;
		return drawinfo->didntCrop;
	}
	
	struct SDrawCommand sCmd;
	sCmd.pTarget = this;
	sCmd.pData = (void*) drawinfo;
	sCmd.list = PVR_LIST_TR_POLY;
	g_videoImp->AddDrawCommand(sCmd, false);
	return drawinfo->didntCrop;
}

float FontInstanceImp::getWidth(const char* str, float size)
{
	float outleft, outdown, height, width;
	cxt->size = size;
	plx_fcxt_str_metrics(cxt, str, &outleft, &height, &width, &outdown);
	return width;
}

void FontInstanceImp::DoDraw(struct SDrawCommand& sCmd)
{
	FontDrawInfo* drawinfo = (FontDrawInfo*) sCmd.pData;
	cxt->size = drawinfo->size;
	
	if (drawinfo->maxWidth > 0)
	{
		float outleft, outup, outright, outdown;
		int32 curnull = strlen(drawinfo->str);
		while (true)
		{
			plx_fcxt_str_metrics(cxt, drawinfo->str, &outleft, &outup, &outright, &outdown);
			if (outright > drawinfo->maxWidth)
			{
				--curnull;
				if (!curnull)
					goto _finish_DoDraw;
				drawinfo->str[curnull] = '\0';
			}
			else
				break;
		}
	}
	
	switch (drawinfo->type)
	{
		//void plx_fcxt_str_metrics(plx_fcxt_t* cxt, const char* str, float* outleft, float* outup, float* outright, float* outdown);
		case FONT_DRAW:
		{
			DrawString(drawinfo->str, cxt, (point_t) {drawinfo->x, drawinfo->y, drawinfo->z}, drawinfo->color);
			break;
		}
		case FONT_DRAW_WITH_BORDER:
		{
			DrawStringWithBorder(drawinfo->str, cxt, (point_t) {drawinfo->x, drawinfo->y, drawinfo->z}, drawinfo->color, drawinfo->borderColor);
			break;
		}
	};
_finish_DoDraw:
	free((void*) drawinfo->str);
	delete drawinfo;
}

// PROTOTYPE

const ClassInfo FontPrototypeImp::info = {"Font", 0, &fontTable, 0};

FontPrototypeImp::FontPrototypeImp(ExecState*, ObjectPrototypeImp* objectProto)
  : FontInstanceImp(objectProto)
{
	Value protect(this);
	setInternalValue(Null());
}

Value FontPrototypeImp::get(ExecState* exec, const Identifier& propertyName) const
{
	return lookupGetFunction<FontProtoFuncImp, ObjectImp>(exec, propertyName, &fontTable, this);
}

FontProtoFuncImp::FontProtoFuncImp(ExecState* exec, int i, int len)
  : InternalFunctionImp(
    static_cast<FunctionPrototypeImp*>(exec->interpreter()->builtinFunctionPrototype().imp())
    ), id(abs(i))
{
	Value protect(this);
	putDirect(lengthPropertyName, len, DontDelete|ReadOnly|DontEnum);
}

//enum { E_toString, E_load, E_unload, E_draw, E_drawWithBorder };

Value FontProtoFuncImp::call(ExecState* exec, Object& thisObj, const List &args)
{
	if (thisObj.className() != "Font")
	{
		Object err = Error::create(exec, TypeError);
		exec->setException(err);
		return err;
	}
	FontInstanceImp* fontImp = (FontInstanceImp*) thisObj.imp();
	switch (id)
	{
		case FontInstanceImp::E_toString:
			return String(fontImp->toString().c_str());
		case FontInstanceImp::E_load:
		{
			if (args.size() > 0)
			{
				float fixed_width = 0;
				if (args.size() > 1)
					fixed_width = args[1].toNumber(exec);
				return Boolean(fontImp->load(args[0].toString(exec).cstring().c_str(), fixed_width));
			}
			break;
		}
		case FontInstanceImp::E_draw:
		{
			if (args.size() > 6)
			{
				return Boolean(fontImp->draw(args[0].toString(exec).cstring().c_str(),
					args[1].toNumber(exec),
					args[2].toNumber(exec),
					args[3].toNumber(exec),
					args[4].toNumber(exec),
					args[5].toNumber(exec),
					args[6].toInteger(exec),
					(args.size() > 7) ? args[7].toBoolean(exec) : true));
			}
			break;
		}
		case FontInstanceImp::E_drawWithBorder:
		{
			if (args.size() > 7)
			{
				return Boolean(fontImp->drawWithBorder(args[0].toString(exec).cstring().c_str(),
					args[1].toNumber(exec),
					args[2].toNumber(exec),
					args[3].toNumber(exec),
					args[4].toNumber(exec),
					args[5].toNumber(exec),
					args[6].toInteger(exec),
					args[7].toInteger(exec),
					(args.size() > 8) ? args[8].toBoolean(exec) : true));
			}
			break;
		}
		case FontInstanceImp::E_getWidth:
		{
			if (args.size() > 1)
			{
				float ftemp = fontImp->getWidth(args[0].toString(exec).cstring().c_str(), args[1].toNumber(exec));
				return Number((float) ftemp);
			}
			break;
		}
		case FontInstanceImp::E_setBilinear:
		{
			if (args.size() > 0)
			{
				plx_txr_setfilter(fontImp->getPlxFont()->txr, args[0].toBoolean(exec) ? PLX_FILTER_BILINEAR : PLX_FILTER_NONE);
			}
			break;
		}
	}
	return Undefined();
}

//OBJECT

FontObjectImp::FontObjectImp(ExecState* exec, FunctionPrototypeImp* funcProto, FontPrototypeImp* fontProto)
  : InternalFunctionImp(funcProto)
{
  Value protect(this);
  
  putDirect(prototypePropertyName, fontProto, DontEnum|DontDelete|ReadOnly);

  // no. of arguments for constructor
  putDirect(lengthPropertyName, 0, ReadOnly|DontDelete|DontEnum);
}

Object FontObjectImp::construct(ExecState* exec, const List& args)
{
	return Object(new FontInstanceImp(g_FontPrototype.imp()));
}

