#ifndef _VIDEO_OBJECT_H_
#define _VIDEO_OBJECT_H_

#include "../kjs/value.h"
#include "../kjs/object.h"
#include "../kjs/types.h"
#include "../kjs/interpreter.h"
#include "punk_array.h"

using namespace KJS;

class VideoFunctionImp;
class DrawTarget;

struct SDrawCommand
{
	DrawTarget* pTarget;
	void* pData;
	pvr_list_t list;
};

class DrawTarget
{
public:
	virtual ~DrawTarget() {};
	virtual void DoDraw(struct SDrawCommand& sCmd) = 0;
};

class VideoImp : public ObjectImp, public DrawTarget
{
public:
	VideoImp();
	
	enum { E_toString, E_frameBegin, E_frameFinish, E_x, E_y, E_width, E_height, E_takeScreenshot,
		   E_drawRect, E_isPAL };

	virtual UString className() const { return "Video"; }
	virtual Value get(ExecState *exec, const Identifier &propertyName) const;
    Value getValueProperty(ExecState *exec, int token) const;
	void put(ExecState *exec, const Identifier &propertyName, const Value &value, int attr = None);
    void putValueProperty(ExecState *exec, int token, const Value &value, int attr = None);
	
	uint32 AddDrawCommand(struct SDrawCommand& sCmd, bool perm);
	void RemoveAllDrawCommands() { punk_array_remove_all(draw_commands); }
	void RemoveDrawCommand(uint32 id);
	void DrawScene();
	
	// It itself is a draw target >:-0
	virtual void DoDraw(struct SDrawCommand& sCmd);
	
	enum { DRAW_TYPE_LINE };
	
	static const ClassInfo info;
	
private:
	DECLARE_PUNK_ARRAY(draw_commands_perm, struct SDrawCommand);
	DECLARE_PUNK_ARRAY(draw_commands, struct SDrawCommand);
};

class VideoFunctionImp : public ObjectImp
{
public:
	VideoFunctionImp(ExecState *exec, int i, int len);
	
	virtual bool implementsCall() const { return true; }
	virtual Value call(ExecState* exec, Object &thisObj, const List &args);
protected:
	uint32 id;
};

void InitVideoObject(ExecState* exec, Object obj);

extern VideoImp* g_videoImp;

#endif
