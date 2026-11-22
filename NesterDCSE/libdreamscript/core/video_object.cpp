#include "video_object.h"
#include "../kjs/lookup.h"
#include "video_object.lut.h"

#include "dc_utils.h"
#include <imageload/imageload.h>

const ClassInfo VideoImp::info = { "Video", 0, &videoTable, 0 };

VideoImp* g_videoImp = NULL;

/* Source for video_object.lut.h

enum { E_toString, E_frameBegin, E_frameFinish, E_x, E_y, E_width, E_height, E_takeScreenshot,
	   E_drawRect, E_isPAL };

@begin videoTable 10
	toString		VideoImp::E_toString		DontDelete|ReadOnly|Function	0
	frameBegin		VideoImp::E_frameBegin		DontDelete|ReadOnly|Function	0
	frameFinish		VideoImp::E_frameFinish		DontDelete|ReadOnly|Function	0
	x				VideoImp::E_x				DontDelete						0
	y				VideoImp::E_y				DontDelete						0
	width			VideoImp::E_width			DontDelete						0
	height			VideoImp::E_height			DontDelete						0
	takeScreenshot	VideoImp::E_takeScreenshot	DontDelete|ReadOnly|Function	1
	drawRect		VideoImp::E_drawRect		DontDelete|ReadOnly|Function	9
	isPAL			VideoImp::E_isPAL			DontDelete|ReadOnly				0
@end
*/

VideoImp::VideoImp() : ObjectImp()
{
	punk_array_init(draw_commands);
	punk_array_init(draw_commands_perm);
}

uint32 VideoImp::AddDrawCommand(struct SDrawCommand& sCmd, bool perm)
{
	if (perm)
	{
		punk_array_push(draw_commands_perm, sCmd);
		return (draw_commands_perm.count - 1) | 0x80000000;
	}
	else
	{
		punk_array_push(draw_commands, sCmd);
		return draw_commands.count - 1;
	}
}

void VideoImp::RemoveDrawCommand(uint32 id)
{
	bool perm = id & 0x80000000;
	id = id & 0x7FFFFFFF;
	if (perm)
	{
		punk_array_remove(draw_commands_perm, id);
	}
	else
	{
		punk_array_remove(draw_commands, id);
	}
}

#define _SUB_DRAW_COMMAND(pa, whichlist)\
	for (k = 0; k < (pa).count; ++k)\
	{\
		if ((pa).data[k].list == (whichlist))\
		{\
			if (!list_opened)\
			{\
				pvr_list_begin((whichlist));\
				list_opened = true;\
			}\
			(pa).data[k].pTarget->DoDraw((pa).data[k]);\
		}\
	}

#define DRAW_COMMAND_LOOP(whichlist)\
	list_opened = false;\
	_SUB_DRAW_COMMAND(draw_commands, whichlist);\
	_SUB_DRAW_COMMAND(draw_commands_perm, whichlist);\
	if (list_opened)\
		pvr_list_finish();

void VideoImp::DrawScene()
{
	bool list_opened;
	uint32 k;
	pvr_wait_ready();
	pvr_scene_begin();
	DRAW_COMMAND_LOOP(PVR_LIST_OP_POLY);
	DRAW_COMMAND_LOOP(PVR_LIST_TR_POLY);
	DRAW_COMMAND_LOOP(PVR_LIST_PT_POLY);
	pvr_scene_finish();
}

Value VideoImp::get(ExecState *exec, const Identifier &propertyName) const
{
	return lookupGet<VideoFunctionImp, VideoImp, ObjectImp>(exec, propertyName, &videoTable, this);
}

/*
struct S_SCREEN_ADJUSTMENTS
{
	int32 x1, y1, x2, y2;
	float xscale, yscale;
};
*/

Value VideoImp::getValueProperty(ExecState *exec, int token) const
{
	switch (token)
	{
		case VideoImp::E_x:
			return Number(screen_adjustments.x1);
		case VideoImp::E_y:
			return Number(screen_adjustments.y1);
		case VideoImp::E_width:
			return Number(screen_adjustments.x2-screen_adjustments.x1);
		case VideoImp::E_height:
			return Number(screen_adjustments.y2-screen_adjustments.y1);
		case VideoImp::E_isPAL:
			return Boolean(vid_mode->flags & VID_PAL);
	};
	return Undefined();
}

void VideoImp::put(ExecState *exec, const Identifier &propertyName, const Value &value, int attr)
{
	lookupPut<VideoImp, ObjectImp>(exec, propertyName, value, attr, &videoTable, this);
}

void VideoImp::putValueProperty(ExecState *exec, int token, const Value &value, int attr)
{
	int tempwidth;
	switch (token)
	{
		case VideoImp::E_x:
			tempwidth = screen_adjustments.x2 - screen_adjustments.x1;
			screen_adjustments.x1 = value.toInteger(exec);
			screen_adjustments.x2 = screen_adjustments.x1 + tempwidth;
			break;
		case VideoImp::E_y:
			tempwidth = screen_adjustments.y2 - screen_adjustments.y1;
			screen_adjustments.y1 = value.toInteger(exec);
			screen_adjustments.y2 = screen_adjustments.y1 + tempwidth;
			break;
		case VideoImp::E_width:
			screen_adjustments.x2 = screen_adjustments.x1 + value.toInteger(exec);
			break;
		case VideoImp::E_height:
			screen_adjustments.y2 = screen_adjustments.y1 + value.toInteger(exec);
			break;
	};
	screen_adjustments.xscale = (((float)screen_adjustments.x2) - ((float)screen_adjustments.x1)) / 640.0f;
	screen_adjustments.yscale = (((float)screen_adjustments.y2) - ((float)screen_adjustments.y1)) / 480.0f;
}

struct S_DRAW_RECT
{
	float x1, y1, x2, y2, z;
	uint32 color;
};

void VideoImp::DoDraw(struct SDrawCommand& sCmd)
{
	//DRAW_TYPE_LINE
	S_DRAW_RECT* s = (S_DRAW_RECT*) sCmd.pData;
	
	pvr_vertex_t vert;
	pvr_poly_cxt_t cxt;
	pvr_poly_hdr_t hdr;

	pvr_poly_cxt_col(&cxt, PVR_LIST_TR_POLY);
	//cxt.gen.culling = PVR_CULLING_NONE;
	pvr_poly_compile(&hdr, &cxt);
	pvr_prim(&hdr, sizeof(hdr));

	vert.flags = PVR_CMD_VERTEX;
	vert.x = screen_adjustments.x1 + (s->x1 * screen_adjustments.xscale);
	vert.y = screen_adjustments.y1 + (s->y2 * screen_adjustments.yscale);
	vert.z = s->z;
	vert.argb = s->color;
	vert.oargb = 0;
	pvr_prim (&vert, sizeof(vert));

	vert.x = screen_adjustments.x1 + (s->x1 * screen_adjustments.xscale);
	vert.y = screen_adjustments.y1 + (s->y1 * screen_adjustments.yscale);
	pvr_prim (&vert, sizeof(vert));

	vert.x = screen_adjustments.x1 + (s->x2 * screen_adjustments.xscale);
	vert.y = screen_adjustments.y1 + (s->y2 * screen_adjustments.yscale);
	pvr_prim (&vert, sizeof(vert));

	vert.flags = PVR_CMD_VERTEX_EOL;
	vert.x = screen_adjustments.x1 + (s->x2 * screen_adjustments.xscale);
	vert.y = screen_adjustments.y1 + (s->y1 * screen_adjustments.yscale);
	pvr_prim (&vert, sizeof(vert));
	
	delete s;
}

VideoFunctionImp::VideoFunctionImp(ExecState* exec, int i, int len) : ObjectImp()
{
	id = i;
	put(exec, lengthPropertyName, Number(len), DontDelete|ReadOnly|DontEnum);
}

Value VideoFunctionImp::call(ExecState *exec, Object &thisObj, const List &args)
{
	if (thisObj.className() != "Video")
	{
		Object err = Error::create(exec, TypeError);
		exec->setException(err);
		return err;
	}
	switch (id)
	{
		case VideoImp::E_toString:
		{
			return String("Video Object");
			break;
		}
		case VideoImp::E_frameBegin:
		{
			g_videoImp->RemoveAllDrawCommands();
			break;
		}
		case VideoImp::E_frameFinish:
		{
			g_videoImp->DrawScene();
			//if (g_doMusicPoll)
			//	snd_stream_poll(music_stream_hnd);
			break;
		}
		case VideoImp::E_takeScreenshot:
		{
			if (args.size() > 0)
			{
				CString str = args[0].toString(exec).cstring();
				vid_screen_shot(str.c_str());
			}
			break;
		}
		case VideoImp::E_drawRect:
		{
			if (args.size() > 5)
			{
				S_DRAW_RECT* s = new S_DRAW_RECT;
				s->x1 = args[0].toNumber(exec);
				s->y1 = args[1].toNumber(exec);
				s->x2 = args[2].toNumber(exec);
				s->y2 = args[3].toNumber(exec);
				s->z = args[4].toNumber(exec);
				s->color = args[5].toUInt32(exec);
				
				struct SDrawCommand sCmd = {g_videoImp, (void*)s, PVR_LIST_TR_POLY};
				g_videoImp->AddDrawCommand(sCmd, false);
			}
			break;
		}
	}
	return Undefined();
}

void InitVideoObject(ExecState* exec, Object obj)
{
	g_videoImp = new VideoImp();
	obj.put(exec, Identifier("Video"), Object(g_videoImp));
}
