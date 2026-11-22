#include "sprite_object.h"
#include "../kjs/error_object.h"
#include "../kjs/operations.h"
#include "../kjs/lookup.h"
#include "image_object.h"
#include "XML.h"
#include "makesplitpath.h"
#include "punk_array.h"

#include "sprite_object.lut.h"

extern "C" {
#include <imageload/imageload.h>
}

using namespace KJS;

Object g_SpritePrototype;
Object g_Sprite;

void InitSpriteClass(ExecState* exec, Object global)
{
	SpritePrototypeImp* spriteProto = new SpritePrototypeImp(exec, (ObjectPrototypeImp*) exec->interpreter()->builtinObjectPrototype().imp());
	g_SpritePrototype = Object(spriteProto);
	g_Sprite = Object(new SpriteObjectImp(exec, (FunctionPrototypeImp*) exec->interpreter()->builtinFunctionPrototype().imp(), spriteProto));
	global.put(exec, "Sprite", g_Sprite, DontEnum|ReadOnly);
}

//INSTANCE

/*

enum { E_toString, E_load, E_unload, E_clone, E_animAddImg, E_animClear, E_animClearAll, E_animation, E_img, E_frame,
E_x, E_y, E_z, E_angle, E_pivotu, E_pivotv, E_scale, E_flippedx, E_flippedy, E_speed, E_visible };

@begin spriteTable 22
	toString		SpriteInstanceImp::E_toString		DontDelete|DontEnum|ReadOnly|Function	0
	load			SpriteInstanceImp::E_load			DontDelete|DontEnum|ReadOnly|Function	1
	unload			SpriteInstanceImp::E_unload			DontDelete|DontEnum|ReadOnly|Function	0
	clone			SpriteInstanceImp::E_clone			DontDelete|DontEnum|ReadOnly|Function	0
	animAddImg		SpriteInstanceImp::E_animAddImg		DontDelete|DontEnum|ReadOnly|Function	3
	animClear		SpriteInstanceImp::E_animClear		DontDelete|DontEnum|ReadOnly|Function	1
	animClearAll	SpriteInstanceImp::E_animClearAll	DontDelete|DontEnum|ReadOnly|Function	0
	animation		SpriteInstanceImp::E_animation		DontDelete								0
	img				SpriteInstanceImp::E_img			DontDelete|ReadOnly						0
	frame			SpriteInstanceImp::E_frame			DontDelete								0
	x				SpriteInstanceImp::E_x				DontDelete								0
	y				SpriteInstanceImp::E_y				DontDelete								0
	z				SpriteInstanceImp::E_z				DontDelete								0
	angle			SpriteInstanceImp::E_angle			DontDelete								0
	pivotu			SpriteInstanceImp::E_pivotu			DontDelete								0
	pivotv			SpriteInstanceImp::E_pivotv			DontDelete								0
	scale			SpriteInstanceImp::E_scale			DontDelete								0
	flippedx		SpriteInstanceImp::E_flippedx		DontDelete								0
	flippedy		SpriteInstanceImp::E_flippedy		DontDelete								0
	speed			SpriteInstanceImp::E_speed			DontDelete								0
	visible			SpriteInstanceImp::E_visible		DontDelete								0
@end
*/

const ClassInfo SpriteInstanceImp::info = {"Sprite", 0, 0, 0};

SpriteInstanceImp::SpriteInstanceImp(ObjectImp* proto)
  : ObjectImp(proto)
{
	frame = 0;
	x = 0;
	y = 0;
	z = 1;
	angle = 0;
	pivotu = 0.5;
	pivotv = 0.5;
	scale = 1;
	flippedx = 0;
	flippedy = 0;
	speed = 1;
	visible = true;
	
	cloneCount = 0;
	pCloneCount = NULL;
	
	curAnim = NULL;
	animCounter = 0;
	
	m_animations = hashmap_new(hash_str, cmp_str, NULL, stdlib_allocator);
	m_images = hashmap_new(hash_str, cmp_str, NULL, stdlib_allocator);
}

SpriteInstanceImp::~SpriteInstanceImp()
{
	unload(true);
}

Value SpriteInstanceImp::get(ExecState *exec, const Identifier &propertyName) const
{
	return lookupGet<SpriteProtoFuncImp, SpriteInstanceImp, ObjectImp>(exec, propertyName, &spriteTable, this);
}

Value SpriteInstanceImp::getValueProperty(ExecState *exec, int token) const
{
	switch (token)
	{
		case SpriteInstanceImp::E_animation:
			return String(animation.c_str());
		case SpriteInstanceImp::E_img:
		{
			if (curAnim)
				return curAnim->frames[frame]->imgObj;
			else
				return Null();
		}
		case SpriteInstanceImp::E_frame:
			return Number(frame);
		case SpriteInstanceImp::E_x:
			return Number(x);
		case SpriteInstanceImp::E_y:
			return Number(y);
		case SpriteInstanceImp::E_z:
			return Number(z);
		case SpriteInstanceImp::E_angle:
			return Number(angle);
		case SpriteInstanceImp::E_pivotu:
			return Number(pivotu);
		case SpriteInstanceImp::E_pivotv:
			return Number(pivotv);
		case SpriteInstanceImp::E_scale:
			return Number(scale);
		case SpriteInstanceImp::E_flippedx:
			return Number(flippedx);
		case SpriteInstanceImp::E_flippedy:
			return Number(flippedy);
		case SpriteInstanceImp::E_speed:
			return Number(speed);
		case SpriteInstanceImp::E_visible:
			return Boolean(visible);
	}
	return Undefined();
}

void SpriteInstanceImp::put(ExecState *exec, const Identifier &propertyName, const Value &value, int attr)
{
	lookupPut<SpriteInstanceImp, ObjectImp>(exec, propertyName, value, attr, &spriteTable, this);
}

void SpriteInstanceImp::putValueProperty(ExecState *exec, int token, const Value &value, int attr)
{
	switch (token)
	{
		case SpriteInstanceImp::E_animation:
		{
			CString tempstr = value.toString(exec).cstring();
			curAnim = (Animation*) hashmap_get(m_animations, tempstr.c_str());
			animation = value.toString(exec).cstring();
			frame = 0;
			break;
		}
		case SpriteInstanceImp::E_frame:
		{
			frame = value.toUInt32(exec);
			break;
		}
		case SpriteInstanceImp::E_x:
		{
			x = value.toNumber(exec);
			break;
		}
		case SpriteInstanceImp::E_y:
		{
			y = value.toNumber(exec);
			break;
		}
		case SpriteInstanceImp::E_z:
		{
			z = value.toNumber(exec);
			break;
		}
		case SpriteInstanceImp::E_angle:
		{
			angle = value.toNumber(exec);
			break;
		}
		case SpriteInstanceImp::E_pivotu:
		{
			pivotu = value.toNumber(exec);
			break;
		}
		case SpriteInstanceImp::E_pivotv:
		{
			pivotv = value.toNumber(exec);
			break;
		}
		case SpriteInstanceImp::E_scale:
		{
			scale = value.toNumber(exec);
			break;
		}
		case SpriteInstanceImp::E_flippedx:
		{
			flippedx = value.toNumber(exec);
			break;
		}
		case SpriteInstanceImp::E_flippedy:
		{
			flippedy = value.toNumber(exec);
			break;
		}
		case SpriteInstanceImp::E_speed:
		{
			speed = value.toNumber(exec);
			break;
		}
		case SpriteInstanceImp::E_visible:
		{
			visible = value.toBoolean(exec);
			break;
		}
	}
}


CString SpriteInstanceImp::toString()
{
	return "Sprite object ";
}

bool SpriteInstanceImp::load(const char* filename, ExecState* exec)
{
	XML* sprXML = XML_OpenFile(filename);
	if (!sprXML)
		return false;
	char szdrive[0x100];
	char szdir[0x100];
	char szfname[0x100];
	char szext[0x10];
	char szTempFilename[0x100];
	_splitpath(filename, szdrive, szdir, szfname, szext);
	// Load Images First
	XML* imagesXML = XML_Find(sprXML, "images", NULL, NULL);
	if (!imagesXML)
	{
		XML_Destroy(sprXML);
		return false;
	}
	uint32 k;
	for (k = 0; k < imagesXML->children.count; ++k)
	{
		XML* imgXML = imagesXML->children.data[k];
		const char* szImageName = XML_GetAttribute(imgXML, "name");
		const char* szImageFile = XML_GetAttribute(imgXML, "file");
		if (!szImageName || !szImageFile)
			continue;
		List locargs;
		Object jsImg = g_Image.construct(exec, locargs);
		ImageInstanceImp* jsImgImp = (ImageInstanceImp*) jsImg.imp();
		sprintf(szTempFilename, "%s/%s", szdir, szImageFile);
		//dbgio_printf("%s\n", szTempFilename);
		if (!jsImgImp->load(szTempFilename, PVR_LIST_PT_POLY, PVR_FILTER_NONE, IMG_ALPHA_KEYED))
			continue;
		hashmap_put(m_images, (void*)szImageName, new Object(jsImg));
	}
	// Then Load Animations and their frames
	XML* animationsXML = XML_Find(sprXML, "animations", NULL, NULL);
	if (!animationsXML)
	{
		XML_Destroy(sprXML);
		return false;
	}
	for (k = 0; k < animationsXML->children.count; ++k)
	{
		XML* animXML = animationsXML->children.data[k];
		const char* szAnimName = XML_GetAttribute(animXML, "name");
		if (!szAnimName)
			continue;
		uint32 n;
		Animation* theAnimThing = new Animation();
		theAnimThing->frames = new AnimFrame*[animXML->children.count];
		memset(theAnimThing->frames, 0, animXML->children.count * sizeof(AnimFrame**));
		for (n = 0; n < animXML->children.count; ++n)
		{
			XML* frameXML = animXML->children.data[n];
			const char* szImgLink = XML_GetAttribute(frameXML, "image");
			if (!szImgLink)
				continue;
			Object* tempImgObj = (Object*) hashmap_get(m_images, szImgLink);
			if (!tempImgObj)
				continue;
			int duration = XML_GetAttributeInt(frameXML, "duration", 1);
			AnimFrame* tempAnimFrame = new AnimFrame(tempImgObj->imp(), (uint32) duration);
			theAnimThing->frames[theAnimThing->frameCount] = tempAnimFrame;
			theAnimThing->frameCount++;
		}
		if (!curAnim)
		{
			curAnim = theAnimThing;
			frame = 0;
		}
		hashmap_put(m_animations, (void*)szAnimName, theAnimThing);
	}
	struct SDrawCommand drawcmd = {this, NULL, PVR_LIST_PT_POLY};
	drawCmdID = g_videoImp->AddDrawCommand(drawcmd, true);
	XML_Destroy(sprXML);
	return true;
}

void SpriteInstanceImp::unload(bool final)
{
	g_videoImp->RemoveDrawCommand(drawCmdID);
	
	iter_t it;
	const char* tempkey;
	hashmap_iterate(m_animations, &it);
	while (tempkey = (const char*) hashmap_next(m_animations, &it))
		delete (Animation*) hashmap_get(m_animations, tempkey);
	hashmap_del(m_animations, NULL, NULL, NULL);
	hashmap_iterate(m_images, &it);
	while (tempkey = (const char*) hashmap_next(m_images, &it))
		delete (Object*) hashmap_get(m_images, tempkey);
	hashmap_del(m_images, NULL, NULL, NULL);
	
	if (!final)
	{
		m_animations = hashmap_new(hash_str, cmp_str, NULL, stdlib_allocator);
		m_images = hashmap_new(hash_str, cmp_str, NULL, stdlib_allocator);
		
		frame = 0;
		x = 0;
		y = 0;
		z = 1;
		angle = 0;
		pivotu = 0.5;
		pivotv = 0.5;
		scale = 1;
		flippedx = 0;
		flippedy = 0;
		speed = 1;
		visible = true;
		curAnim = NULL;
		animCounter = 0;
	}
}

SpriteInstanceImp* SpriteInstanceImp::clone()
{
	return NULL;
}

void SpriteInstanceImp::animAddImg(const char* name, ImageInstanceImp* img, uint32 framesDuration)
{
	
}

void SpriteInstanceImp::animClear(const char* name)
{
	
}

void SpriteInstanceImp::animClearAll()
{
	
}

void SpriteInstanceImp::DoDraw(struct SDrawCommand& sCmd)
{
	if (visible && curAnim)
	{
		AnimFrame* pFrame = curAnim->frames[frame];
		ImageInstanceImp* imgImp = (ImageInstanceImp*) pFrame->imgObj.imp();
		Image* pImg = imgImp->GetImage();
		Image_DrawRotated(pImg, x, y, scale, 0xffffffff, z, pivotu, pivotv, angle);
		
		//dbgio_printf("animCounter = %f speed = %f\n", animCounter, speed);
		animCounter += speed;
		if (animCounter >= pFrame->duration)
		{
			frame++;
			if (frame >= curAnim->frameCount)
				frame = 0;
			animCounter -= ((float) pFrame->duration);
		}
	}
}

// PROTOTYPE

const ClassInfo SpritePrototypeImp::info = {"Sprite", 0, &spriteTable, 0};

SpritePrototypeImp::SpritePrototypeImp(ExecState*, ObjectPrototypeImp* objectProto)
  : SpriteInstanceImp(objectProto)
{
	Value protect(this);
	setInternalValue(Null());
}

Value SpritePrototypeImp::get(ExecState* exec, const Identifier& propertyName) const
{
	return lookupGetFunction<SpriteProtoFuncImp, ObjectImp>(exec, propertyName, &spriteTable, this);
}

SpriteProtoFuncImp::SpriteProtoFuncImp(ExecState* exec, int i, int len)
  : InternalFunctionImp(
    static_cast<FunctionPrototypeImp*>(exec->interpreter()->builtinFunctionPrototype().imp())
    ), id(abs(i))
{
	Value protect(this);
	putDirect(lengthPropertyName, len, DontDelete|ReadOnly|DontEnum);
}

Value SpriteProtoFuncImp::call(ExecState* exec, Object& thisObj, const List &args)
{
	if (thisObj.className() != "Sprite")
	{
		Object err = Error::create(exec, TypeError);
		exec->setException(err);
		return err;
	}
	SpriteInstanceImp* spr = (SpriteInstanceImp*) thisObj.imp();
	switch (id)
	{
		case SpriteInstanceImp::E_toString:
			return String(spr->toString().c_str());
		case SpriteInstanceImp::E_load:
		{
			if (args.size() < 1)
				return Boolean((bool)false);
			return Boolean(spr->load(args[0].toString(exec).cstring().c_str(), exec));
		}
		case SpriteInstanceImp::E_unload:
		{
			spr->unload();
			return Undefined();
		}
		case SpriteInstanceImp::E_clone:
		{
			SpriteInstanceImp* temp = spr->clone();
			if (temp)
				return Object(temp);
			else
				return Null();
		}
		case SpriteInstanceImp::E_animAddImg:
		{
			if (args.size() < 3)
				return Boolean((bool)false);
			Object tempobj = args[1].toObject(exec);
			if (thisObj.className() != "Image")
			{
				Object err = Error::create(exec, TypeError);
				exec->setException(err);
				return err;
			}
			spr->animAddImg(args[0].toString(exec).cstring().c_str(), (ImageInstanceImp*) tempobj.imp(), args[2].toInteger(exec));
			return Undefined();
		}
		case SpriteInstanceImp::E_animClear:
		{
			if (args.size() < 1)
				return Boolean((bool)false);
			spr->animClear(args[0].toString(exec).cstring().c_str());
			return Undefined();
		}
		case SpriteInstanceImp::E_animClearAll:
		{
			spr->animClearAll();
			return Undefined();
		}
	}
	return Undefined();
}

//OBJECT

SpriteObjectImp::SpriteObjectImp(ExecState* exec, FunctionPrototypeImp* funcProto, SpritePrototypeImp* spriteProto)
  : InternalFunctionImp(funcProto)
{
  Value protect(this);
  
  putDirect(prototypePropertyName, spriteProto, DontEnum|DontDelete|ReadOnly);

  // no. of arguments for constructor
  putDirect(lengthPropertyName, 0, ReadOnly|DontDelete|DontEnum);
}

Object SpriteObjectImp::construct(ExecState* exec, const List& args)
{
	return Object(new SpriteInstanceImp(g_SpritePrototype.imp()));
}

