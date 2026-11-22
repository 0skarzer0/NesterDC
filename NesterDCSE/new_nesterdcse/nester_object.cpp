#include "nester_object.h"
#include <kjs/lookup.h>
#include "nester_object.lut.h"
#include "nes.h"
#include "null_sound_mgr.h"
#include "dc_sound_mgr.h"
#include <core/hashmap.h>
#include <core/image_object.h>
#include "main.h"
#include "dc_saving.h"
#include "visualization.h"
#include <math.h>
#include "dc_sound_mgr.h"
#include "dc_sound.h"

#define DRAW_NES_SCREEN		0
#define DRAW_CROSSHAIR		1

DECLARE_PUNK_ARRAY(g_gameGenieCodes, char*)

#include <core/dc_utils.h>

const ClassInfo NesterImp::info = { "Nester", 0, &nesterTable, 0 };

NesterImp* g_nesterImp = NULL;

Value g_nsfInfo = Null();
Value g_visData = Null();

static volatile bool bGotVisData = false;

void removeAllGGCodes();

static volatile unsigned int emulated_frames = 0;
static volatile uint32 frameCount = 0;
static volatile uint32 fps = 0;
static volatile uint32 nextfps = 0;
uint32 refreshRate = 60;

int32 zeroSoundCount = 0;

/* Source for nester_object.lut.h

enum { E_toString, E_loadROM, E_emulateFrame, E_bilinearFiltering, E_saveState, E_loadState, E_reset, E_softReset,
	   E_getROMCRC, E_getROMCRCstr, E_saveSlot, E_exControllerType, E_draw, E_getButtonMapping, E_setButtonMapping,
	   E_getUseAnalog, E_setUseAnalog, E_getAutofireSpeed, E_setAutofireSpeed, E_saveSRAM, E_hasSRAM,
	   E_nsfPlay, E_nsfInfo, E_visData, E_frozen, E_zeroSoundCount, E_clearSound, E_autoLoop,
	   E_autoStart, E_autoEnd, E_fps, E_drawTrue, E_setPan, E_getPan, E_bgNSFStart, E_bgNSFEnd,
	   E_emulatedFrames };

@begin nesterTable 32
	toString			NesterImp::E_toString				DontDelete|ReadOnly|Function	0
	loadROM				NesterImp::E_loadROM				DontDelete|ReadOnly|Function	1
	emulateFrame		NesterImp::E_emulateFrame			DontDelete|ReadOnly|Function	0
	bilinearFiltering	NesterImp::E_bilinearFiltering		DontDelete						0
	saveState			NesterImp::E_saveState				DontDelete|ReadOnly|Function	0
	loadState			NesterImp::E_loadState				DontDelete|ReadOnly|Function	0
	reset				NesterImp::E_reset					DontDelete|ReadOnly|Function	0
	softReset			NesterImp::E_softReset				DontDelete|ReadOnly|Function	0
	getROMCRC			NesterImp::E_getROMCRC				DontDelete|ReadOnly|Function	0
	getROMCRCstr		NesterImp::E_getROMCRCstr			DontDelete|ReadOnly|Function	0
	saveSlot			NesterImp::E_saveSlot				DontDelete						0
	exControllerType	NesterImp::E_exControllerType		DontDelete|ReadOnly				0
	draw				NesterImp::E_draw					DontDelete|ReadOnly|Function	5
	getButtonMapping	NesterImp::E_getButtonMapping		DontDelete|ReadOnly|Function	2
	setButtonMapping	NesterImp::E_setButtonMapping		DontDelete|ReadOnly|Function	3
	getUseAnalog		NesterImp::E_getUseAnalog			DontDelete|ReadOnly|Function	1
	setUseAnalog		NesterImp::E_setUseAnalog			DontDelete|ReadOnly|Function	2
	getAutofireSpeed	NesterImp::E_getAutofireSpeed		DontDelete|ReadOnly|Function	2
	setAutofireSpeed	NesterImp::E_setAutofireSpeed		DontDelete|ReadOnly|Function	3
	saveSRAM			NesterImp::E_saveSRAM				DontDelete|ReadOnly|Function	0
	hasSRAM				NesterImp::E_hasSRAM				DontDelete|ReadOnly				0
	nsfPlay				NesterImp::E_nsfPlay				DontDelete|ReadOnly|Function	1
	nsfInfo				NesterImp::E_nsfInfo				DontDelete|ReadOnly				0
	visData				NesterImp::E_visData				DontDelete|ReadOnly				0
	frozen				NesterImp::E_frozen					DontDelete						0
	zeroSoundCount		NesterImp::E_zeroSoundCount			DontDelete|ReadOnly				0
	clearSound			NesterImp::E_clearSound				DontDelete|ReadOnly|Function	0
	autoLoop			NesterImp::E_autoLoop				DontDelete|ReadOnly|Function	0
	autoStart			NesterImp::E_autoStart				DontDelete|ReadOnly|Function	0
	autoEnd				NesterImp::E_autoEnd				DontDelete|ReadOnly|Function	0
	fps					NesterImp::E_fps					DontDelete|ReadOnly				0
	drawTrue			NesterImp::E_drawTrue				DontDelete|ReadOnly|Function	0
	setPan				NesterImp::E_setPan					DontDelete|ReadOnly|Function	2
	getPan				NesterImp::E_getPan					DontDelete|ReadOnly|Function	1
	bgNSFStart			NesterImp::E_bgNSFStart				DontDelete|ReadOnly|Function	0
	bgNSFEnd			NesterImp::E_bgNSFEnd				DontDelete|ReadOnly|Function	0
	emulatedFrames		NesterImp::E_emulatedFrames			DontDelete|ReadOnly				0
@end
*/

NesterImp::NesterImp() : ObjectImp()
{
	uint32 k = 0;
	for (; k < BAR_COUNT; ++k)
		falloffVals[k] = 0;
	zeroSoundCount = 0;
	fps = 0;
	nextfps = 0;
	frameCount = 0;
}

Value NesterImp::get(ExecState *exec, const Identifier &propertyName) const
{
	return lookupGet<NesterFunctionImp, NesterImp, ObjectImp>(exec, propertyName, &nesterTable, this);
}

Value NesterImp::getValueProperty(ExecState *exec, int token) const
{
	switch (token)
	{
		case E_bilinearFiltering:
			return Boolean(bilinear_filtering);
		case E_saveSlot:
			return Number(g_save_slot);
		case E_exControllerType:
			return Number(main_emu->ex_controller_type);
		case E_hasSRAM:
			return Boolean(main_emu->has_sram());
		case E_nsfInfo:
			return g_nsfInfo;
		case E_visData:
		{
			if (!bGotVisData)
			{
				dc_sound_mgr* dcsnd = (dc_sound_mgr*) main_emu->get_snd_mgr();
				List locargs;
				bool bZero = true;
				for (uint32 k = 0; k < 6; ++k)
				{
					int16 itemp = dcsnd->getAvg(k);
					//printf("%d ", itemp);
					if (itemp > 200)
						bZero = false;
					locargs.append(Number(itemp));
				}
				//printf("\n");
				if (bZero)
					zeroSoundCount++;
				Object temparray(exec->interpreter()->builtinArray().construct(exec, locargs));
				g_visData = temparray;
				bGotVisData = true;
			}
			return g_visData;
		}
		case E_frozen:
			return Boolean(main_emu->frozen());
		case E_zeroSoundCount:
			return Number(zeroSoundCount);
		case E_fps:
			return Number(fps);
		case E_emulatedFrames:
			return Number(emulated_frames);
	}
	return Undefined();
}

void NesterImp::put(ExecState *exec, const Identifier &propertyName, const Value &value, int attr)
{
	lookupPut<NesterImp, ObjectImp>(exec, propertyName, value, attr, &nesterTable, this);
}

void NesterImp::putValueProperty(ExecState *exec, int token, const Value &value, int attr)
{
	switch (token)
	{
		case E_bilinearFiltering:
			bilinear_filtering = value.toBoolean(exec);
			break;
		case E_saveSlot:
			g_save_slot = value.toInteger(exec);
			break;
		case E_frozen:
		{
			bool b = value.toBoolean(exec);
			bool eb = main_emu->frozen();
			if (b && !eb)
				main_emu->freeze();
			else if (!b && eb)
				main_emu->thaw();
			break;
		}
	}
}

#define TYPE_SCREEN			0
#define TYPE_SCREEN_TRUE	1

struct SNesterDraw
{
	uint8 type;
	uint32 color;
	float x, y, z, width, height;
};

uint32 barColorTable[16] = {
	0xFF00FF00, // green
	0xFF1FFF00,
	0xFF3EFF00,
	0xFF5DFF00,
	0xFF7CFF00,
	0xFF9BFF00,
	0xFFBAFF00,
	0xFFFFFF00, // yellow
	0xFFFFBA00,
	0xFFFF9B00,
	0xFFFF7C00,
	0xFFFF5D00,
	0xFFFF3E00,
	0xFFFF1F00,
	0xFFFF0000, // red
	0xFFFF0000
};

void updateFPS()
{
	frameCount++;
	if ((frameCount%refreshRate) == 0)
	{
		fps = nextfps;
		nextfps = 0;
	}
}

void NesterImp::DoDraw(struct SDrawCommand& sCmd)
{
	SNesterDraw* drawInfo = (SNesterDraw*) sCmd.pData;
	
	switch (drawInfo->type)
	{
		case TYPE_SCREEN:
		{
			pvr_dr_state_t dr_state;
			pvr_vertex_t* vert;

			pvr_poly_hdr_t *poly;
			if (bilinear_filtering)
				poly = &nes_pvr_poly_header_filter_bilinear[text_counter];
			else
				poly = &nes_pvr_poly_header_filter_none[text_counter];

			pvr_prim (poly, sizeof(pvr_poly_hdr_t));

			pvr_dr_init(dr_state);

			vert = pvr_dr_target(dr_state);
			vert->flags = PVR_CMD_VERTEX;
			vert->x = screen_adjustments.x1 + (drawInfo->x * screen_adjustments.xscale);
			vert->y = screen_adjustments.y1 + ((drawInfo->y + drawInfo->height) * screen_adjustments.yscale);
			vert->z = drawInfo->z;
			vert->u = 0;
			vert->v = 224.0f/256.0f;
			vert->argb = drawInfo->color;
			vert->oargb = 0;
			pvr_dr_commit(vert);

			vert = pvr_dr_target(dr_state);
			vert->flags = PVR_CMD_VERTEX;
			vert->x = screen_adjustments.x1 + (drawInfo->x * screen_adjustments.xscale);
			vert->y = screen_adjustments.y1 + (drawInfo->y * screen_adjustments.yscale);
			vert->z = drawInfo->z;
			vert->u = 0;
			vert->v = 0;
			vert->argb = drawInfo->color;
			vert->oargb = 0;
			pvr_dr_commit(vert);

			vert = pvr_dr_target(dr_state);
			vert->flags = PVR_CMD_VERTEX;
			vert->x = screen_adjustments.x1 + ((drawInfo->x + drawInfo->width) * screen_adjustments.xscale);
			vert->y = screen_adjustments.y1 + ((drawInfo->y + drawInfo->height) * screen_adjustments.yscale);
			vert->z = drawInfo->z;
			vert->u = 1.0;
			vert->v = 224.0f/256.0f;
			vert->argb = drawInfo->color;
			vert->oargb = 0;
			pvr_dr_commit(vert);

			vert = pvr_dr_target(dr_state);
			vert->flags = PVR_CMD_VERTEX_EOL;
			vert->x = screen_adjustments.x1 + ((drawInfo->x + drawInfo->width) * screen_adjustments.xscale);
			vert->y = screen_adjustments.y1 + (drawInfo->y * screen_adjustments.yscale);
			vert->z = drawInfo->z;
			vert->u = 1.0;
			vert->v = 0;
			vert->argb = drawInfo->color;
			vert->oargb = 0;
			pvr_dr_commit(vert);
			break;
		}
		case TYPE_SCREEN_TRUE:
		{
#define 	TRUE_X1	((640-512)/2)
#define 	TRUE_Y1	((480-448)/2)
#define 	TRUE_X2	(TRUE_X1+512)
#define 	TRUE_Y2	(TRUE_Y1+448)

			pvr_dr_state_t dr_state;
			pvr_vertex_t* vert;

			pvr_poly_hdr_t *poly;
			poly = &nes_pvr_poly_header_filter_none[text_counter];

			pvr_prim (poly, sizeof(pvr_poly_hdr_t));

			pvr_dr_init(dr_state);

			vert = pvr_dr_target(dr_state);
			vert->flags = PVR_CMD_VERTEX;
			vert->x = TRUE_X1;
			vert->y = TRUE_Y2;
			vert->z = drawInfo->z;
			vert->u = 0;
			vert->v = 224.0f/256.0f;
			vert->argb = drawInfo->color;
			vert->oargb = 0;
			pvr_dr_commit(vert);

			vert = pvr_dr_target(dr_state);
			vert->flags = PVR_CMD_VERTEX;
			vert->x = TRUE_X1;
			vert->y = TRUE_Y1;
			vert->z = drawInfo->z;
			vert->u = 0;
			vert->v = 0;
			vert->argb = drawInfo->color;
			vert->oargb = 0;
			pvr_dr_commit(vert);

			vert = pvr_dr_target(dr_state);
			vert->flags = PVR_CMD_VERTEX;
			vert->x = TRUE_X2;
			vert->y = TRUE_Y2;
			vert->z = drawInfo->z;
			vert->u = 1.0;
			vert->v = 224.0f/256.0f;
			vert->argb = drawInfo->color;
			vert->oargb = 0;
			pvr_dr_commit(vert);

			vert = pvr_dr_target(dr_state);
			vert->flags = PVR_CMD_VERTEX_EOL;
			vert->x = TRUE_X2;
			vert->y = TRUE_Y1;
			vert->z = drawInfo->z;
			vert->u = 1.0;
			vert->v = 0;
			vert->argb = drawInfo->color;
			vert->oargb = 0;
			pvr_dr_commit(vert);
			break;
		}
	}
	
	delete drawInfo;
}

NesterFunctionImp::NesterFunctionImp(ExecState* exec, int i, int len) : ObjectImp()
{
	id = i;
	put(exec, lengthPropertyName, Number(len), DontDelete|ReadOnly|DontEnum);
}

void NesterFunctionImp::draw(NesterImp* imp, uint32 color, float x, float y, float z, float width, float height)
{
	SNesterDraw* drawInfo = new SNesterDraw;
	drawInfo->type = TYPE_SCREEN;
	drawInfo->color = color;
	drawInfo->x = x;
	drawInfo->y = y;
	drawInfo->z = z;
	drawInfo->width = width;
	drawInfo->height = height;
	
	struct SDrawCommand sCmd;
	sCmd.pTarget = imp;
	sCmd.pData = (void*) drawInfo;
	sCmd.list = PVR_LIST_OP_POLY;
	g_videoImp->AddDrawCommand(sCmd, false);
}

void NesterFunctionImp::drawTrue(NesterImp* imp, uint32 color, float z)
{
	SNesterDraw* drawInfo = new SNesterDraw;
	drawInfo->type = TYPE_SCREEN_TRUE;
	drawInfo->color = color;
	drawInfo->z = z;

	struct SDrawCommand sCmd;
	sCmd.pTarget = imp;
	sCmd.pData = (void*) drawInfo;
	sCmd.list = PVR_LIST_OP_POLY;
	g_videoImp->AddDrawCommand(sCmd, false);
}

static volatile int rest_frames_obj = 0;

static void timer_handler_obj(irq_t source, irq_context_t *context)
{
	++rest_frames_obj;
	updateFPS();
}

static volatile bool inbgNSF = false;
bool bgNSFon = false;

static void timer_handler_bgNSF(irq_t source, irq_context_t *context)
{
	if (inbgNSF)
		return;
	inbgNSF = true;
	main_emu->emulate_frame_skip();
	++emulated_frames;
	bGotVisData = false;
	inbgNSF = false;
}

Value NesterFunctionImp::call(ExecState *exec, Object &thisObj, const List &args)
{
	if (thisObj.className() != "Nester")
	{
		Object err = Error::create(exec, TypeError);
		exec->setException(err);
		return err;
	}
	switch (id)
	{
		case NesterImp::E_toString:
		{
			return String("Nester Object");
			break;
		}
		case NesterImp::E_loadROM:
		{
			if (args.size() > 0)
			{
				CString str = args[0].toString(exec).cstring();
				bool success = LoadROM(str.c_str());
				if (success)
				{
					if (main_emu->getNES_ROM()->isNSF)
					{
						NSF_HEADER* nsf_header = &(main_emu->getNES_ROM()->nsf_header);
						Object tempobj(exec->interpreter()->builtinObject().construct(exec, List::empty()));
						tempobj.put(exec, Identifier("Version"), Number(nsf_header->Version));
						tempobj.put(exec, Identifier("TotalSongs"), Number(nsf_header->TotalSongs));
						tempobj.put(exec, Identifier("StartingSong"), Number(nsf_header->StartingSong));
						tempobj.put(exec, Identifier("Name"), String(nsf_header->SongName));
						tempobj.put(exec, Identifier("Artist"), String(nsf_header->Artist));
						tempobj.put(exec, Identifier("Copyright"), String(nsf_header->Copyright));
						g_nsfInfo = tempobj;
					}
					else
						g_nsfInfo = Null();
					removeAllGGCodes();
				}
				if (success)
					emulated_frames = 0;
				return Boolean(success);
			}
			break;
		}
		case NesterImp::E_emulateFrame:
		{
			if (main_emu)
			{
				bGotVisData = false;
				bool renderScreen = true;
				if (args.size() > 0)
					renderScreen = args[0].toBoolean(exec);
				if (main_emu->frozen())
				{
					just_entered_game = true;
					main_emu->thaw();
				}
				
				if (renderScreen)
				{
					int pollResult = poll_input(main_emu);
					main_emu->emulate_frame(nes_screen);
					nextfps++;
					text_counter++;
					if (text_counter == 2)
						text_counter = 0;
					dc_nesscreen_copy_texture_320x240to256x256(nes_texture_addr[text_counter]);
					if (pollResult)
						main_emu->freeze();
					return Number(pollResult);
				}
				else
				{
					main_emu->emulate_frame_skip();
				}
				++emulated_frames;
				return Number(0);
			}
			break;
		}
		case NesterImp::E_autoStart:
		{
			refreshRate = (vid_mode->flags & VID_PAL)?50:60;
			if (args.size() > 0)
				refreshRate = args[0].toUInt32(exec);
			timer_prime (TMU2, refreshRate, true);
			int sample_rate = main_emu->get_snd_mgr()->get_sample_rate();
    		int sample_bits = main_emu->get_snd_mgr()->get_sample_bits();
			apu_setparams(sample_rate, refreshRate, sample_bits);
			irq_set_handler (EXC_TMU2_TUNI2, timer_handler_obj);
			timer_start (TMU2);
			rest_frames_obj = 1;
			break;
		}
		case NesterImp::E_autoEnd:
		{
			timer_stop (TMU2);
			break;
		}
		case NesterImp::E_autoLoop:
		{
			if (main_emu)
			{
				if (refreshRate != ((vid_mode->flags & VID_PAL)?50:60) && rest_frames_obj == 0)
					return Number(2);
				if (main_emu->frozen())
				{
					just_entered_game = true;
					main_emu->thaw();
				}
				int pollResult = poll_input(main_emu);
				main_emu->emulate_frame(nes_screen);
				++emulated_frames;
				nextfps++;
				text_counter++;
				if (text_counter == 2)
					text_counter = 0;
				dc_nesscreen_copy_texture_320x240to256x256(nes_texture_addr[text_counter]);
				if (--rest_frames_obj > 0)
				{
					int drop_frames = rest_frames_obj;
					rest_frames_obj = 0;
					while (drop_frames--)
					{
						//static int skipcount = 0;
						//printf("skipcount=%d\n", skipcount++);
						main_emu->emulate_frame_skip();
						++emulated_frames;
						int temp = poll_input(main_emu);
						if (pollResult == 0)
							pollResult = temp;
					}
				}
				if (pollResult)
					main_emu->freeze();
				return Number(pollResult);
			}
			break;
		}
		case NesterImp::E_drawTrue:
		{
			if (args.size() == 0)
			{
				drawTrue((NesterImp*) thisObj.imp(), 0xFFFFFFFF, 10);
				Object crossObj = Object::dynamicCast(thisObj.get(exec, Identifier("crosshairImg")));
				if ((main_emu->ex_controller_type == NES::EX_OPTICAL_GUN || main_emu->ex_controller_type == NES::EX_SPACE_SHADOW_GUN) && !crossObj.isNull() && strcmp(crossObj.classInfo()->className, "Image") == 0)
				{
					ImageInstanceImp* crossImp = (ImageInstanceImp*) crossObj.imp();
					float gunx = TRUE_X1+(main_emu->NES_gun.x*2);
					float guny = TRUE_Y1+(main_emu->NES_gun.y*2);
					gunx -= (crossImp->getWidth() / 2);
					guny -= (crossImp->getHeight() / 2);
					crossImp->drawTrue(gunx, guny, 1, 0xFFFFFFFF, 50);
				}
			}
			else
			{
				uint32 color = 0xFFFFFFFF;
				if (args.size() > 0)
					color = args[0].toUInt32(exec);
				drawTrue((NesterImp*) thisObj.imp(), color, 10);
			}
			break;
		}
		case NesterImp::E_draw:
		{
			if (args.size() == 0)
			{
				draw((NesterImp*) thisObj.imp(), 0xFFFFFFFF, 0, 0, 10, 640, 480);
				
				Object crossObj = Object::dynamicCast(thisObj.get(exec, Identifier("crosshairImg")));
				if ((main_emu->ex_controller_type == NES::EX_OPTICAL_GUN || main_emu->ex_controller_type == NES::EX_SPACE_SHADOW_GUN) && !crossObj.isNull() && strcmp(crossObj.classInfo()->className, "Image") == 0)
				{
					ImageInstanceImp* crossImp = (ImageInstanceImp*) crossObj.imp();
					float gunx = main_emu->NES_gun.x * 640.0/256;
					float guny = main_emu->NES_gun.y * 2;
					gunx -= (crossImp->getWidth() / 2);
					guny -= (crossImp->getHeight() / 2);
					crossImp->draw(gunx, guny, 1, 0xFFFFFFFF, 50);
				}
			}
			else
			{
				uint32 color = 0xFFFFFFFF;
				float x = 0;
				float y = 0;
				float z = 10;
				float width = 640;
				float height = 480;
				switch (args.size())
				{
					case 6:
						height = args[5].toNumber(exec);
					case 5:
						width = args[4].toNumber(exec);
					case 4:
						z = args[3].toNumber(exec);
					case 3:
						y = args[2].toNumber(exec);
					case 2:
						x = args[1].toNumber(exec);
					case 1:
						color = args[0].toUInt32(exec);
				};
				draw((NesterImp*) thisObj.imp(), color, x, y, z, width, height);
			}
			break;
		}
		case NesterImp::E_saveState:
		{
			SaveState();
			break;
		}
		case NesterImp::E_loadState:
		{
			LoadState();
			break;
		}
		case NesterImp::E_reset:
		{
			main_emu->reset();
			emulated_frames = 0;
			break;
		}
		case NesterImp::E_softReset:
		{
			main_emu->softreset();
			emulated_frames = 0;
			break;
		}
		case NesterImp::E_getROMCRC:
		{
			return Number(main_emu->crc32());
			break;
		}
		case NesterImp::E_getROMCRCstr:
		{
			char temp[10];
			sprintf(temp, "%X", main_emu->crc32());
			return String(temp);
			break;
		}
		case NesterImp::E_getButtonMapping:
		{
			if (args.size() > 1)
			{
				uint32 whichController = args[0].toUInt32(exec);
				uint32 whichButton = args[1].toUInt32(exec);
				return Number(g_button_map[whichController][whichButton]);
			}
			break;
		}
		case NesterImp::E_setButtonMapping:
		{
			if (args.size() > 2)
			{
				uint32 whichController = args[0].toUInt32(exec);
				uint32 whichButton = args[1].toUInt32(exec);
				g_button_map[whichController][whichButton] = args[2].toUInt32(exec);
			}
			break;
		}
		case NesterImp::E_getUseAnalog:
		{
			if (args.size() > 0)
			{
				uint32 whichController = args[0].toUInt32(exec);
				return Boolean(use_analog[whichController]);
			}
			break;
		}
		case NesterImp::E_setUseAnalog:
		{
			if (args.size() > 1)
			{
				uint32 whichController = args[0].toUInt32(exec);
				use_analog[whichController] = args[1].toBoolean(exec);
			}
			break;
		}
		case NesterImp::E_getAutofireSpeed:
		{
			if (args.size() > 1)
			{
				uint32 whichController = args[0].toUInt32(exec);
				uint32 whichButton = args[1].toUInt32(exec);
				if (whichButton == CONTROLLER_NES_B)
					return Number(autofire_b_speed[whichController]);
				else if (whichButton == CONTROLLER_NES_A)
					return Number(autofire_a_speed[whichController]);
			}
			break;
		}
		case NesterImp::E_setAutofireSpeed:
		{
			if (args.size() > 2)
			{
				uint32 whichController = args[0].toUInt32(exec);
				uint32 whichButton = args[1].toUInt32(exec);
				if (whichButton == CONTROLLER_NES_B)
					autofire_b_speed[whichController] = args[2].toUInt32(exec);
				else if (whichButton == CONTROLLER_NES_A)
					autofire_a_speed[whichController] = args[2].toUInt32(exec);
			}
			break;
		}
		case NesterImp::E_saveSRAM:
		{
			sram_save_to_vmu(vmu_dev, main_emu);
			break;
		}
		case NesterImp::E_nsfPlay:
		{
			uint32 whichTrack = args[0].toUInt32(exec);
			null_sound_mgr nully;
			main_snd_mgr->clear_buffer();
			main_emu->new_snd_mgr(&nully);
			main_emu->reset();
			NES_pad* pad = main_emu->get_pad(0 & 0x03);
			for (uint32 k = 0; k < ((whichTrack + 1) * 2); ++k)
			{
				pad->nes_START = (k % 2) ? FALSE : TRUE;
				main_emu->emulate_frame_skip();
			}
			pad->nes_START = FALSE;
			main_emu->emulate_frame_skip();
			main_emu->new_snd_mgr(main_snd_mgr);
			zeroSoundCount = 0;
			emulated_frames = 0;
			break;
		}
		case NesterImp::E_clearSound:
		{
			if (main_snd_mgr)
			{
				main_snd_mgr->clear_buffer();
			}
			else
				dbgio_printf("sjkdlfj\n");
			break;
		}
		case NesterImp::E_setPan:
		{
			if (args.size() > 1)
			{
				uint32 whichChannel = args[0].toUInt32(exec);
				uint32 panVal = (args[1].toUInt32(exec)/2)+64;
				dc_sound_set_pan(whichChannel, panVal);
			}
			break;
		}
		case NesterImp::E_getPan:
		{
			if (args.size() > 0)
				return Number((dc_sound_get_pan(args[0].toUInt32(exec))-64)*2);
			break;
		}
		case NesterImp::E_bgNSFStart:
		{
			timer_prime(TMU2, 60, true);
			irq_set_handler(EXC_TMU2_TUNI2, timer_handler_bgNSF);
			timer_start(TMU2);
			bgNSFon = true;
			break;
		}
		case NesterImp::E_bgNSFEnd:
		{
			if (bgNSFon)
			{
				timer_stop (TMU2);
				while (inbgNSF);
				bgNSFon = false;
			}
			break;
		}
	}
	return Undefined();
}

GameGenieFunctionImp::GameGenieFunctionImp(ExecState* exec, int i, int len) : ObjectImp()
{
	id = i;
	put(exec, lengthPropertyName, Number(len), DontDelete|ReadOnly|DontEnum);
}

//enum { E_addCode, E_removeCode, E_removeAllCodes, E_getCodeCount, E_getCode };

void updateGGCodes()
{
	main_emu->clear_genie();
	uint32 k;
	for (k = 0; k < g_gameGenieCodes.count; ++k)
		main_emu->load_genie(g_gameGenieCodes.data[k]);
	main_emu->set_genie();
}

void removeAllGGCodes()
{
	main_emu->clear_genie();
	uint32 k;
	for (k = 0; k < g_gameGenieCodes.count; ++k)
		delete g_gameGenieCodes.data[k];
	punk_array_remove_all(g_gameGenieCodes);
}

Value GameGenieFunctionImp::call(ExecState *exec, Object &thisObj, const List &args)
{
	switch (id)
	{
		case GameGenieFunctionImp::E_addCode:
		{
			if (args.size() > 0)
			{
				CString str = args[0].toString(exec).cstring();
				const char* sztemp = str.c_str();
				char* szNew = new char[strlen(sztemp)+1];
				strcpy(szNew, sztemp);
				punk_array_push(g_gameGenieCodes, szNew);
				updateGGCodes();
			}
			break;
		}
		case GameGenieFunctionImp::E_removeCode:
		{
			if (args.size() > 0)
			{
				CString str = args[0].toString(exec).cstring();
				uint32 k;
				for (k = 0; k < g_gameGenieCodes.count; ++k)
				{
					if (str == g_gameGenieCodes.data[k])
					{
						delete g_gameGenieCodes.data[k];
						punk_array_remove(g_gameGenieCodes, k);
						break;
					}
				}
				updateGGCodes();
			}
			break;
		}
		case GameGenieFunctionImp::E_removeAllCodes:
		{
			removeAllGGCodes();
			break;
		}
		case GameGenieFunctionImp::E_getCodeCount:
		{
			return Number(g_gameGenieCodes.count);
			break;
		}
		case GameGenieFunctionImp::E_getCode:
		{
			if (args.size() > 0)
				return String(g_gameGenieCodes.data[args[0].toInteger(exec)]);
			break;
		}
	}
	return Undefined();
}

#define CONT_L			(CONT_DPAD2_RIGHT << 1)
#define CONT_R			(CONT_DPAD2_RIGHT << 2)
#define EMPTY_MAPPING	(CONT_DPAD2_RIGHT << 3)

void InitNesterObject(ExecState* exec, Object obj)
{
	punk_array_init(g_gameGenieCodes);
	
	g_nesterImp = new NesterImp();
	Object nesterObj(g_nesterImp);
	obj.put(exec, Identifier("Nester"), nesterObj);
	
	Object gameGenieObj(exec->interpreter()->builtinObject().construct(exec, List::empty()));
	gameGenieObj.put(exec, Identifier("addCode"), Object(new GameGenieFunctionImp(exec, GameGenieFunctionImp::E_addCode, 1)));
	gameGenieObj.put(exec, Identifier("removeCode"), Object(new GameGenieFunctionImp(exec, GameGenieFunctionImp::E_removeCode, 1)));
	gameGenieObj.put(exec, Identifier("removeAllCodes"), Object(new GameGenieFunctionImp(exec, GameGenieFunctionImp::E_removeAllCodes, 0)));
	gameGenieObj.put(exec, Identifier("getCodeCount"), Object(new GameGenieFunctionImp(exec, GameGenieFunctionImp::E_getCodeCount, 0)));
	gameGenieObj.put(exec, Identifier("getCode"), Object(new GameGenieFunctionImp(exec, GameGenieFunctionImp::E_getCode, 1)));
	
	nesterObj.put(exec, Identifier("GameGenie"), gameGenieObj);
	
	nesterObj.put(exec, Identifier("crosshairImg"), Null());
	
    obj.put(exec, Identifier("EX_NONE"), Number(NES::EX_NONE));
    obj.put(exec, Identifier("EX_FAMILY_KEYBOARD"), Number(NES::EX_FAMILY_KEYBOARD));
    obj.put(exec, Identifier("EX_OPTICAL_GUN"), Number(NES::EX_OPTICAL_GUN));
    obj.put(exec, Identifier("EX_SPACE_SHADOW_GUN"), Number(NES::EX_SPACE_SHADOW_GUN));
    obj.put(exec, Identifier("EX_HYPER_SHOT"), Number(NES::EX_HYPER_SHOT));
    obj.put(exec, Identifier("EX_CRAZY_CLIMBER"), Number(NES::EX_CRAZY_CLIMBER));
    obj.put(exec, Identifier("EX_ARKANOID_PADDLE"), Number(NES::EX_ARKANOID_PADDLE));
    obj.put(exec, Identifier("EX_MAHJONG"), Number(NES::EX_MAHJONG));
    
    obj.put(exec, Identifier("CONT_DPAD_UP"), Number(CONT_DPAD_UP), ReadOnly|DontEnum);
    obj.put(exec, Identifier("CONT_DPAD_DOWN"), Number(CONT_DPAD_DOWN), ReadOnly|DontEnum);
    obj.put(exec, Identifier("CONT_DPAD_LEFT"), Number(CONT_DPAD_LEFT), ReadOnly|DontEnum);
    obj.put(exec, Identifier("CONT_DPAD_RIGHT"), Number(CONT_DPAD_RIGHT), ReadOnly|DontEnum);
    obj.put(exec, Identifier("CONT_A"), Number(CONT_A), ReadOnly|DontEnum);
    obj.put(exec, Identifier("CONT_B"), Number(CONT_B), ReadOnly|DontEnum);
    obj.put(exec, Identifier("CONT_X"), Number(CONT_X), ReadOnly|DontEnum);
    obj.put(exec, Identifier("CONT_Y"), Number(CONT_Y), ReadOnly|DontEnum);
    obj.put(exec, Identifier("CONT_START"), Number(CONT_START), ReadOnly|DontEnum);
    obj.put(exec, Identifier("CONT_L"), Number(CONT_L), ReadOnly|DontEnum);
    obj.put(exec, Identifier("CONT_R"), Number(CONT_R), ReadOnly|DontEnum);
    obj.put(exec, Identifier("EMPTY_MAPPING"), Number(EMPTY_MAPPING), ReadOnly|DontEnum);
    
    obj.put(exec, Identifier("NES_B"), Number(CONTROLLER_NES_B), ReadOnly|DontEnum);
    obj.put(exec, Identifier("NES_A"), Number(CONTROLLER_NES_A), ReadOnly|DontEnum);
    obj.put(exec, Identifier("NES_SELECT"), Number(CONTROLLER_NES_SELECT), ReadOnly|DontEnum);
    obj.put(exec, Identifier("NES_START"), Number(CONTROLLER_NES_START), ReadOnly|DontEnum);
    obj.put(exec, Identifier("NES_COIN"), Number(CONTROLLER_NES_COIN), ReadOnly|DontEnum);
    obj.put(exec, Identifier("NES_B_AUTOFIRE"), Number(CONTROLLER_NES_B_AUTOFIRE), ReadOnly|DontEnum);
    obj.put(exec, Identifier("NES_A_AUTOFIRE"), Number(CONTROLLER_NES_A_AUTOFIRE), ReadOnly|DontEnum);
    obj.put(exec, Identifier("NES_QUICK_SAVE"), Number(CONTROLLER_QUICK_SAVE), ReadOnly|DontEnum);
    obj.put(exec, Identifier("NES_QUICK_LOAD"), Number(CONTROLLER_QUICK_LOAD), ReadOnly|DontEnum);
    obj.put(exec, Identifier("NES_COUNT"), Number(CONTROLLER_NES_COUNT), ReadOnly|DontEnum);
    
   	obj.put(exec, Identifier("AUTOFIRE_SLOW"), Number(24), ReadOnly|DontEnum);
   	obj.put(exec, Identifier("AUTOFIRE_MEDIUM"), Number(12), ReadOnly|DontEnum);
   	obj.put(exec, Identifier("AUTOFIRE_FAST"), Number(6), ReadOnly|DontEnum);
   	obj.put(exec, Identifier("AUTOFIRE_INSANE"), Number(2), ReadOnly|DontEnum);
   	
	obj.put(exec, Identifier("RECTANGLE1_CHANNEL"), Number(0), ReadOnly|DontEnum);
	obj.put(exec, Identifier("RECTANGLE2_CHANNEL"), Number(1), ReadOnly|DontEnum);
	obj.put(exec, Identifier("TRIANGLE_CHANNEL"), Number(2), ReadOnly|DontEnum);
	obj.put(exec, Identifier("NOISE_CHANNEL"), Number(3), ReadOnly|DontEnum);
	obj.put(exec, Identifier("DMC_CHANNEL"), Number(4), ReadOnly|DontEnum);
	obj.put(exec, Identifier("EX_CHANNEL"), Number(5), ReadOnly|DontEnum);
}
