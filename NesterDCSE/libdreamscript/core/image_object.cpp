#include "image_object.h"
#include "../kjs/error_object.h"
#include "../kjs/operations.h"
#include "../kjs/lookup.h"

#include "image_object.lut.h"

extern "C" {
#include <imageload/imageload.h>
}

using namespace KJS;

Object g_ImagePrototype;
Object g_Image;

void InitImageClass(ExecState* exec, Object global)
{
	ImagePrototypeImp* imageProto = new ImagePrototypeImp(exec, (ObjectPrototypeImp*) exec->interpreter()->builtinObjectPrototype().imp());
	g_ImagePrototype = Object(imageProto);
	g_Image = Object(new ImageObjectImp(exec, (FunctionPrototypeImp*) exec->interpreter()->builtinFunctionPrototype().imp(), imageProto));
	global.put(exec, "Image", g_Image, DontEnum|ReadOnly);
}

//INSTANCE

const ClassInfo ImageInstanceImp::info = {"Image", 0, 0, 0};

int ImageInstanceImp::loadCount = 0;

ImageInstanceImp::ImageInstanceImp(ObjectImp* proto)
  : ObjectImp(proto)
{
	Image_Init(&img);
	loaded = false;
}

CString ImageInstanceImp::toString()
{
	CString temp = "Image object: ";
	temp += filename;
	return temp;
}

bool ImageInstanceImp::load(const char* fn, pvr_list_t drawlist, int filtering, IMG_ALPHA alpha)
{
	unload();
	bool result = Image_Load(&img, fn, drawlist, filtering, alpha) != 0;
	if (result)
	{
		filename = fn;
		loaded = true;
	}
	return result;
}

void ImageInstanceImp::unload()
{
	if (loaded)
	{
		Image_Destroy(&img);
		Image_Init(&img);
		loaded = false;
	}
}

struct SImageDrawData
{
	uint32 func;
	float x;
	float y;
	float scale;
	float width;
	uint32 color;
	float z;
	float height;
	float pivotu;
	float pivotv;
	float angle;
};

void ImageInstanceImp::draw(float x, float y, float scale, uint32 color, float z)
{
	struct SImageDrawData* pData = new struct SImageDrawData;
	pData->func = ImageProtoFuncImp::E_draw;
	pData->x = x;
	pData->y = y;
	pData->scale = scale;
	pData->color = color;
	pData->z = z;
	
	struct SDrawCommand sCmd;
	sCmd.pTarget = this;
	sCmd.pData = pData;
	sCmd.list = img.list;
	g_videoImp->AddDrawCommand(sCmd, false);
}

void ImageInstanceImp::drawTrue(float x, float y, float scale, uint32 color, float z)
{
	struct SImageDrawData* pData = new struct SImageDrawData;
	pData->func = ImageProtoFuncImp::E_drawTrue;
	pData->x = x;
	pData->y = y;
	pData->scale = scale;
	pData->color = color;
	pData->z = z;
	
	struct SDrawCommand sCmd;
	sCmd.pTarget = this;
	sCmd.pData = pData;
	sCmd.list = img.list;
	g_videoImp->AddDrawCommand(sCmd, false);
}

void ImageInstanceImp::drawWH(float x, float y, float width, float height, uint32 color, float z)
{
	struct SImageDrawData* pData = new struct SImageDrawData;
	pData->func = ImageProtoFuncImp::E_drawWH;
	pData->x = x;
	pData->y = y;
	pData->width = width;
	pData->height = height;
	pData->color = color;
	pData->z = z;

	struct SDrawCommand sCmd;
	sCmd.pTarget = this;
	sCmd.pData = pData;
	sCmd.list = img.list;
	g_videoImp->AddDrawCommand(sCmd, false);
}

void ImageInstanceImp::drawRotated(float x, float y, float scale, uint32 color, float z,
					   float pivotu, float pivotv, float angle)
{
	struct SImageDrawData* pData = new struct SImageDrawData;
	pData->func = ImageProtoFuncImp::E_drawRotated;
	pData->x = x;
	pData->y = y;
	pData->scale = scale;
	pData->color = color;
	pData->z = z;
	pData->pivotu = pivotu;
	pData->pivotv = pivotv;
	pData->angle = angle;

	struct SDrawCommand sCmd;
	sCmd.pTarget = this;
	sCmd.pData = pData;
	sCmd.list = img.list;
	g_videoImp->AddDrawCommand(sCmd, false);
}

const char* ImageInstanceImp::getFilename()
{
	return filename.c_str();
}

uint32 ImageInstanceImp::getWidth()
{
	return img.width;
}

uint32 ImageInstanceImp::getHeight()
{
	return img.height;
}

void ImageInstanceImp::DoDraw(struct SDrawCommand& sCmd)
{
	struct SImageDrawData* pData = (struct SImageDrawData*) sCmd.pData;
	switch (pData->func)
	{
		case ImageProtoFuncImp::E_draw:
		{
			Image_Draw(&img, pData->x, pData->y, pData->scale, pData->color, pData->z);
			break;
		}
		case ImageProtoFuncImp::E_drawTrue:
		{
			Image_DrawTrue(&img, pData->x, pData->y, pData->scale, pData->color, pData->z);
			break;
		}
		case ImageProtoFuncImp::E_drawWH:
		{
			Image_DrawWH(&img, pData->x, pData->y, pData->width, pData->height, pData->color, pData->z);
			break;
		}
		case ImageProtoFuncImp::E_drawRotated:
		{
			Image_DrawRotated(&img, pData->x, pData->y, pData->scale, pData->color, pData->z, pData->pivotu,
							  pData->pivotv, pData->angle);
			break;
		}
	};
	delete pData;
}

// PROTOTYPE

const ClassInfo ImagePrototypeImp::info = {"Image", 0, &imageProtoTable, 0};

/*

enum { E_toString, E_load, E_unload, E_draw, E_drawWH, E_getFilename, E_getWidth, E_getHeight };

@begin imageProtoTable 10
	toString		ImageProtoFuncImp::E_toString		DontEnum|Function	0
	load			ImageProtoFuncImp::E_load			DontEnum|Function	1
	unload			ImageProtoFuncImp::E_unload			DontEnum|Function	0
	draw			ImageProtoFuncImp::E_draw			DontEnum|Function	0
	drawTrue		ImageProtoFuncImp::E_drawTrue		DontEnum|Function	0
	drawWH			ImageProtoFuncImp::E_drawWH			DontEnum|Function	0
	drawRotated		ImageProtoFuncImp::E_drawRotated	DontEnum|Function	0
	getFilename		ImageProtoFuncImp::E_getFilename	DontEnum|Function	0
	getWidth		ImageProtoFuncImp::E_getWidth		DontEnum|Function	0
	getHeight		ImageProtoFuncImp::E_getHeight		DontEnum|Function	0
@end
*/

ImagePrototypeImp::ImagePrototypeImp(ExecState*, ObjectPrototypeImp* objectProto)
  : ImageInstanceImp(objectProto)
{
	Value protect(this);
	setInternalValue(Null());
}

Value ImagePrototypeImp::get(ExecState* exec, const Identifier& propertyName) const
{
	return lookupGetFunction<ImageProtoFuncImp, ObjectImp>(exec, propertyName, &imageProtoTable, this);
}

ImageProtoFuncImp::ImageProtoFuncImp(ExecState* exec, int i, int len)
  : InternalFunctionImp(
    static_cast<FunctionPrototypeImp*>(exec->interpreter()->builtinFunctionPrototype().imp())
    ), id(abs(i))
{
	Value protect(this);
	putDirect(lengthPropertyName, len, DontDelete|ReadOnly|DontEnum);
}

Value ImageProtoFuncImp::call(ExecState* exec, Object& thisObj, const List &args)
{
	if (thisObj.className() != "Image")
	{
		Object err = Error::create(exec, TypeError);
		exec->setException(err);
		return err;
	}
	ImageInstanceImp* img = (ImageInstanceImp*) thisObj.imp();
	switch (id)
	{
		case ImageProtoFuncImp::E_toString:
			return String(img->toString().c_str());
		case ImageProtoFuncImp::E_load:
		{
			if (args.size() < 1)
				return Boolean((bool)false);
			pvr_list_t list;
			if (args.size() < 2)
				list = PVR_LIST_OP_POLY;
			else
				list = args[1].toInteger(exec);
			int filtering;
			if (args.size() < 3)
				filtering = PVR_FILTER_NONE;
			else
				filtering = args[2].toInteger(exec);
			IMG_ALPHA alpha;
			if (args.size() < 4)
				alpha = IMG_ALPHA_NONE;
			else
				alpha = (IMG_ALPHA) args[3].toInteger(exec);
			return Boolean(img->load(args[0].toString(exec).cstring().c_str(), list, filtering, alpha));
		}
		case ImageProtoFuncImp::E_unload:
			img->unload();
			return thisObj;
		case ImageProtoFuncImp::E_draw:
		{
			img->draw(args[0].toNumber(exec), //x
					  args[1].toNumber(exec), //y
					  args[2].toNumber(exec), //scale
					  args[3].toUInt32(exec), //color
					  args[4].toNumber(exec)  //z
				);
			return thisObj;
		}
		case ImageProtoFuncImp::E_drawWH:
		{
			img->drawWH(args[0].toNumber(exec), //x
					  args[1].toNumber(exec),   //y
					  args[2].toNumber(exec),   //width
					  args[3].toNumber(exec),   //height
					  args[4].toUInt32(exec),   //color
					  args[5].toNumber(exec)    //z
				);
			return thisObj;
		}
		case ImageProtoFuncImp::E_drawRotated:
		{
			img->drawRotated(args[0].toNumber(exec), //x
					  args[1].toNumber(exec),   //y
					  args[2].toNumber(exec),   //scale
					  args[3].toUInt32(exec),   //color
					  args[4].toNumber(exec),   //z
					  args[5].toNumber(exec),   //pivotu
					  args[6].toNumber(exec),   //pivotv
					  args[7].toNumber(exec)    //angle
				);
			return thisObj;
		}
		case ImageProtoFuncImp::E_getFilename:
		{
			return String(img->getFilename());
		}
		case ImageProtoFuncImp::E_getWidth:
		{
			return Number(img->getWidth());
		}
		case ImageProtoFuncImp::E_getHeight:
		{
			return Number(img->getHeight());
		}
	}
	return Undefined();
}

//OBJECT

ImageObjectImp::ImageObjectImp(ExecState* exec, FunctionPrototypeImp* funcProto, ImagePrototypeImp* imageProto)
  : InternalFunctionImp(funcProto)
{
  Value protect(this);
  
  putDirect(prototypePropertyName, imageProto, DontEnum|DontDelete|ReadOnly);

  // no. of arguments for constructor
  putDirect(lengthPropertyName, 0, ReadOnly|DontDelete|DontEnum);
}

Object ImageObjectImp::construct(ExecState* exec, const List& args)
{
	return Object(new ImageInstanceImp(g_ImagePrototype.imp()));
}

