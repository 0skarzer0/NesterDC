#ifndef _NESTER_OBJECT_H_
#define _NESTER_OBJECT_H_

#include <kjs/value.h>
#include <kjs/object.h>
#include <kjs/types.h>
#include <kjs/interpreter.h>
#include <core/punk_array.h>
#include <core/video_object.h>

using namespace KJS;

class NesterFunctionImp;

#define BAR_COUNT	19
//#define BAR_COUNT	76

class NesterImp : public ObjectImp, public DrawTarget
{
public:
	NesterImp();
	
	enum { E_toString, E_loadROM, E_emulateFrame, E_bilinearFiltering, E_saveState, E_loadState, E_reset, E_softReset,
		   E_getROMCRC, E_getROMCRCstr, E_saveSlot, E_exControllerType, E_draw, E_getButtonMapping, E_setButtonMapping,
		   E_getUseAnalog, E_setUseAnalog, E_getAutofireSpeed, E_setAutofireSpeed, E_saveSRAM, E_hasSRAM,
		   E_nsfPlay, E_nsfInfo, E_visData, E_frozen, E_zeroSoundCount, E_clearSound, E_autoLoop,
		   E_autoStart, E_autoEnd, E_fps, E_drawTrue, E_setPan, E_getPan, E_bgNSFStart, E_bgNSFEnd,
		   E_emulatedFrames };

	virtual UString className() const { return "Nester"; }
	virtual Value get(ExecState *exec, const Identifier &propertyName) const;
    Value getValueProperty(ExecState *exec, int token) const;
	void put(ExecState *exec, const Identifier &propertyName, const Value &value, int attr = None);
    void putValueProperty(ExecState *exec, int token, const Value &value, int attr = None);
	
	static const ClassInfo info;
	
	virtual void DoDraw(struct SDrawCommand& sCmd);
	
	void calcVisData(int8* intern_vis_data) const;
	
protected:
	int32 falloffVals[BAR_COUNT];
};

class NesterFunctionImp : public ObjectImp
{
public:
	NesterFunctionImp(ExecState *exec, int i, int len);
	
	virtual bool implementsCall() const { return true; }
	virtual Value call(ExecState* exec, Object &thisObj, const List &args);
	
	void drawTrue(NesterImp* imp, uint32 color, float z);
	void draw(NesterImp* imp, uint32 color, float x, float y, float z, float width, float height);
	void drawVis(NesterImp* imp, float x, float y, float width, float height, float z);
	
protected:
	uint32 id;
};

class GameGenieFunctionImp : public ObjectImp
{
public:
	GameGenieFunctionImp(ExecState *exec, int i, int len);
	
	enum { E_addCode, E_removeCode, E_removeAllCodes, E_getCodeCount, E_getCode };
	
	virtual bool implementsCall() const { return true; }
	virtual Value call(ExecState* exec, Object &thisObj, const List &args);
protected:
	uint32 id;
};

void InitNesterObject(ExecState* exec, Object obj);

extern NesterImp* g_nesterImp;

#endif
