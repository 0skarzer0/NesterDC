#ifndef _SPRITE_OBJECT_H_
#define _SPRITE_OBJECT_H_

#include "../kjs/value.h"
#include "../kjs/internal.h"
#include "../kjs/function_object.h"
#include "image.h"
#include "video_object.h"
#include "hashmap.h"

//#include <sys/time.h>
#include "../kjs/port_kjs.h"

using namespace KJS;

class ImageInstanceImp;

class AnimFrame
{
public:
	AnimFrame(ObjectImp* imgImp, uint32 dur) : imgObj(imgImp), duration(dur) {}
	
	Object imgObj;
	uint32 duration;
};

class Animation
{
public:
	Animation()
	{
		frames = NULL;
		frameCount = 0;
	}
	
	~Animation()
	{
		if (frames)
		{
			uint32 k;
			for (k = 0; k < frameCount; ++k)
				delete frames[k];
			delete[] frames;
		}
	}
	
	AnimFrame** frames;
	uint32 frameCount;
};

class SpriteInstanceImp : public ObjectImp, public DrawTarget
{
public:
	SpriteInstanceImp(ObjectImp* proto);
	virtual ~SpriteInstanceImp();

	enum { E_toString, E_load, E_unload, E_clone, E_animAddImg, E_animClear, E_animClearAll, E_animation, E_img, E_frame,\
			E_x, E_y, E_z, E_angle, E_pivotu, E_pivotv, E_scale, E_flippedx, E_flippedy, E_speed, E_visible };

	virtual const ClassInfo* classInfo() const { return &info; }
	virtual Value get(ExecState *exec, const Identifier &propertyName) const;
    Value getValueProperty(ExecState *exec, int token) const;
	void put(ExecState *exec, const Identifier &propertyName, const Value &value, int attr = None);
    void putValueProperty(ExecState *exec, int token, const Value &value, int attr = None);
	
	// Start JS Functions
	CString toString();
	
	bool load(const char* filename, ExecState* exec);
	void unload(bool final = false);
	SpriteInstanceImp* clone();
	void animAddImg(const char* name, ImageInstanceImp* img, uint32 framesDuration);
	void animClear(const char* name);
	void animClearAll();
	// End JS Functions
	
	//DrawTarget
	virtual void DoDraw(struct SDrawCommand& sCmd);
	
	static const ClassInfo info;
	
protected:
	// JS properties
	CString animation;
	uint32 frame;
	float x;
	float y;
	float z;
	float angle;
	float pivotu;
	float pivotv;
	float scale;
	float flippedx;
	float flippedy;
	float speed;
	bool visible;
	// END JS properties
	
	int32 cloneCount;
	int32* pCloneCount;
	
	Animation* curAnim;
	float animCounter;
	
	struct hashmap* m_images;
	struct hashmap* m_animations;
	
	uint32 drawCmdID;
};

class SpritePrototypeImp : public SpriteInstanceImp
{
public:
	SpritePrototypeImp(ExecState* exec, ObjectPrototypeImp* objectProto);
	Value get(ExecState *exec, const Identifier &p) const;
	virtual const ClassInfo* classInfo() const { return &info; }
	static const ClassInfo info;
};

class SpriteProtoFuncImp : public InternalFunctionImp
{
public:
	SpriteProtoFuncImp(ExecState* exec, int i, int len);

	virtual bool implementsCall() const { return true; }
	virtual Value call(ExecState* exec, Object& thisObj, const List& args);
private:
	int id;
};

class SpriteObjectImp : public InternalFunctionImp
{
public:
	SpriteObjectImp(ExecState* exec, FunctionPrototypeImp* funcProto, SpritePrototypeImp* dateProto);

	virtual bool implementsConstruct() const { return true; }
	virtual Object construct(ExecState* exec, const List &args);

	Object construct(const List &);
};

void InitSpriteClass(ExecState* exec, Object globobj);

#endif
