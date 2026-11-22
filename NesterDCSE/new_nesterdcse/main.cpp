#include <kos.h>
#include "nes.h"
#include "/usr/local/dc/kos1.3/kos/kernel/arch/dreamcast/hardware/pvr/pvr_internal.h"
#include "dc_saving.h"
#include "null_sound_mgr.h"
#include "dc_sound_mgr.h"
#include "png/png.h"
#include <core/image.h>

#include <dreamscript.h>
#include <core/dc_mouse.h>
#include <core/dc_controller.h>

#include "nester_object.h"
#include "nesdb_object.h"

#include "main.h"

#define ANALOG_THRESHOLD	(128 / 2)

using namespace KJS;

#define ARGB_PACK_COLOR(a, r, g, b) ( \
	( ((uint8)(a)) << 24 ) | \
	( ((uint8)(r)) << 16 ) | \
	( ((uint8)(g)) << 8 ) | \
	( ((uint8)(b)) << 0 ) )

const char *progname = "***NesterDC***";

uint8 keymap_jp106_dc2nes[256] = {
	NES_device_kbd::NES_KEY_VoidSymbol,
	NES_device_kbd::NES_KEY_VoidSymbol,
	NES_device_kbd::NES_KEY_VoidSymbol,
	NES_device_kbd::NES_KEY_VoidSymbol,

	NES_device_kbd::NES_KEY_A,	 /* 0x04 */
	NES_device_kbd::NES_KEY_B,
	NES_device_kbd::NES_KEY_C,
	NES_device_kbd::NES_KEY_D,
	NES_device_kbd::NES_KEY_E,
	NES_device_kbd::NES_KEY_F,
	NES_device_kbd::NES_KEY_G,
	NES_device_kbd::NES_KEY_H,
	NES_device_kbd::NES_KEY_I,
	NES_device_kbd::NES_KEY_J,
	NES_device_kbd::NES_KEY_K,
	NES_device_kbd::NES_KEY_L,
	NES_device_kbd::NES_KEY_M,
	NES_device_kbd::NES_KEY_N,
	NES_device_kbd::NES_KEY_O,
	NES_device_kbd::NES_KEY_P,
	NES_device_kbd::NES_KEY_Q,
	NES_device_kbd::NES_KEY_R,
	NES_device_kbd::NES_KEY_S,
	NES_device_kbd::NES_KEY_T,
	NES_device_kbd::NES_KEY_U,
	NES_device_kbd::NES_KEY_V,
	NES_device_kbd::NES_KEY_W,
	NES_device_kbd::NES_KEY_X,
	NES_device_kbd::NES_KEY_Y,
	NES_device_kbd::NES_KEY_Z,

	NES_device_kbd::NES_KEY_1,	/* 0x1e */
	NES_device_kbd::NES_KEY_2,
	NES_device_kbd::NES_KEY_3,
	NES_device_kbd::NES_KEY_4,
	NES_device_kbd::NES_KEY_5,
	NES_device_kbd::NES_KEY_6,
	NES_device_kbd::NES_KEY_7,
	NES_device_kbd::NES_KEY_8,
	NES_device_kbd::NES_KEY_9,
	NES_device_kbd::NES_KEY_0,

	NES_device_kbd::NES_KEY_Return,	/* 0x28 */
	NES_device_kbd::NES_KEY_Escape,
	NES_device_kbd::NES_KEY_Delete,	/* BACKSPACE */
	NES_device_kbd::NES_KEY_VoidSymbol, 	/* TAB */
	NES_device_kbd::NES_KEY_Space,
	NES_device_kbd::NES_KEY_minus,
	NES_device_kbd::NES_KEY_caret,	/* PLUS (CARET on jp106) */
	NES_device_kbd::NES_KEY_at,		/* LBRACKET (AT on jp106) */
	NES_device_kbd::NES_KEY_bracketleft,	/* RBRACKET (LBRACKET on jp106) */
	NES_device_kbd::NES_KEY_VoidSymbol,	/* BACKSLASH (UNKNOWN on jp106) */
	NES_device_kbd::NES_KEY_bracketright, /* UNKNOWN (RBACKET on jp106) */
	NES_device_kbd::NES_KEY_semicolon,
	NES_device_kbd::NES_KEY_colon,	/* QUOTE (COLON on jp106) */
	NES_device_kbd::NES_KEY_VoidSymbol,   /* TILDE (UNKNOWN on jp106) */

	NES_device_kbd::NES_KEY_comma,	/* 0x36 */
	NES_device_kbd::NES_KEY_period,
	NES_device_kbd::NES_KEY_slash,
	NES_device_kbd::NES_KEY_Control_L,	/* CAPSLOCK */

	NES_device_kbd::NES_KEY_F1,	/* 0x3a */
	NES_device_kbd::NES_KEY_F2,
	NES_device_kbd::NES_KEY_F3,
	NES_device_kbd::NES_KEY_F4,
	NES_device_kbd::NES_KEY_F5,
	NES_device_kbd::NES_KEY_F6,
	NES_device_kbd::NES_KEY_F7,
	NES_device_kbd::NES_KEY_F8,
	NES_device_kbd::NES_KEY_VoidSymbol,	/* F9 */
	NES_device_kbd::NES_KEY_VoidSymbol,	/* F10 */
	NES_device_kbd::NES_KEY_VoidSymbol,	/* F11 */
	NES_device_kbd::NES_KEY_Kana,

	NES_device_kbd::NES_KEY_VoidSymbol,	/* PRINT */ /* 0x46 */
	NES_device_kbd::NES_KEY_VoidSymbol,	/* SCRLOCK */
	NES_device_kbd::NES_KEY_VoidSymbol,	/* PAUSE */
	NES_device_kbd::NES_KEY_Insert,
	NES_device_kbd::NES_KEY_Clear,
	NES_device_kbd::NES_KEY_VoidSymbol,	/* PGUP */
	NES_device_kbd::NES_KEY_Delete,
	NES_device_kbd::NES_KEY_Stop,
	NES_device_kbd::NES_KEY_VoidSymbol,	/* PGDOWN */
	NES_device_kbd::NES_KEY_Right,
	NES_device_kbd::NES_KEY_Left,
	NES_device_kbd::NES_KEY_Down,
	NES_device_kbd::NES_KEY_Up,

	NES_device_kbd::NES_KEY_VoidSymbol,	/* PAD_NUMLOCK */ /* 0x53 */
	NES_device_kbd::NES_KEY_slash,
	NES_device_kbd::NES_KEY_VoidSymbol,	/* PAD_MULTIPLY */
	NES_device_kbd::NES_KEY_minus,
	NES_device_kbd::NES_KEY_VoidSymbol,	/* PAD_PLUS */
	NES_device_kbd::NES_KEY_Return,
	NES_device_kbd::NES_KEY_1,
	NES_device_kbd::NES_KEY_2,
	NES_device_kbd::NES_KEY_3,
	NES_device_kbd::NES_KEY_4,
	NES_device_kbd::NES_KEY_5,
	NES_device_kbd::NES_KEY_6,
	NES_device_kbd::NES_KEY_7,
	NES_device_kbd::NES_KEY_8,
	NES_device_kbd::NES_KEY_9,
	NES_device_kbd::NES_KEY_0,
	NES_device_kbd::NES_KEY_period,
	NES_device_kbd::NES_KEY_VoidSymbol, /* S3 */
};

uint8* keymap_dc2nes;

void keymap_dc2nes_init()
{
	keymap_jp106_dc2nes[0x87] = NES_device_kbd::NES_KEY_apostrophe;
	keymap_jp106_dc2nes[0x89] = NES_device_kbd::NES_KEY_backslash;
	keymap_dc2nes = keymap_jp106_dc2nes;
}

uint16 nes_screen[320 * 240] __attribute__ ((aligned(32)));

pvr_ptr_t nes_texture_addr[2];
pvr_poly_hdr_t nes_pvr_poly_header_filter_none[2];
pvr_poly_hdr_t nes_pvr_poly_header_filter_bilinear[2];
pvr_ptr_t dc_texture_addr;
pvr_poly_hdr_t dc_pvr_poly_header;

bool8 bilinear_filtering = TRUE;

uint32 text_counter = 0;

bool just_entered_game = false;

char rom_filename[0x100];
char rom_title[0x100];

NES* main_emu = NULL;
sound_mgr* main_snd_mgr = NULL;
dc_sound_mgr* dc_main_snd_mgr = NULL;

maple_device_t* vmu_dev = NULL;

uint32 g_button_map[4][CONTROLLER_NES_COUNT] = {

{ CONT_X, CONT_A, CONT_R, CONT_START, CONT_L, CONT_Y, CONT_B, EMPTY_MAPPING, EMPTY_MAPPING },
{ CONT_X, CONT_A, CONT_R, CONT_START, CONT_L, CONT_Y, CONT_B, EMPTY_MAPPING, EMPTY_MAPPING },
{ CONT_X, CONT_A, CONT_R, CONT_START, CONT_L, CONT_Y, CONT_B, EMPTY_MAPPING, EMPTY_MAPPING },
{ CONT_X, CONT_A, CONT_R, CONT_START, CONT_L, CONT_Y, CONT_B, EMPTY_MAPPING, EMPTY_MAPPING }

};

uint32 autofire_b[4] = {0, 0, 0, 0};
uint32 autofire_a[4] = {0, 0, 0, 0};

uint32 autofire_a_speed[4] = {2, 2, 2, 2};
uint32 autofire_b_speed[4] = {2, 2, 2, 2};

bool8 interrupt_game_loop = false;
bool8 use_analog[4] = {FALSE, FALSE, FALSE, FALSE};

int g_save_slot = 0;

pvr_init_params_t params = {
	{
		PVR_BINSIZE_16,	// opaque polygons
		PVR_BINSIZE_0,	// opaque modifiers
		PVR_BINSIZE_16,	// translucent polygons
		PVR_BINSIZE_0,	// translucent modifiers
		PVR_BINSIZE_16	// punch-thrus
	},
	512*1024,			// Vertex buffer size
	0					// DMA
	};

//extern uint8 romdisk[];
//KOS_INIT_ROMDISK(romdisk);

bool8 FindFirstVMU(char* outPath)
{
	file_t fpd = fs_open("/vmu", O_DIR | O_RDONLY);
	if (!fpd)
		return FALSE;
	dirent_t* dirinfo = fs_readdir(fpd);
	if (dirinfo)
	{
		sprintf(outPath, "/vmu/%s", dirinfo->name);
		fs_close(fpd);
		return TRUE;
	}
	fs_close(fpd);
	return FALSE;
}

void texture_init()
{
	pvr_poly_cxt_t poly;
	pvr_ptr_t addr;

	int k = 0;
	for (k = 0; k < 2; ++k)
	{
		addr = pvr_mem_malloc(256 * 256 * 2);
		nes_texture_addr[k] = addr;
		pvr_poly_cxt_txr(&poly, PVR_LIST_OP_POLY, PVR_TXRFMT_RGB565 | PVR_TXRFMT_NONTWIDDLED, 256, 256, addr, PVR_FILTER_NONE);
		pvr_poly_compile(&nes_pvr_poly_header_filter_none[k], &poly);
		pvr_poly_cxt_txr(&poly, PVR_LIST_OP_POLY, PVR_TXRFMT_RGB565 | PVR_TXRFMT_NONTWIDDLED, 256, 256, addr, PVR_FILTER_BILINEAR);
		pvr_poly_compile(&nes_pvr_poly_header_filter_bilinear[k], &poly);
	}
}

void SaveQuickState()
{
	if (!main_emu)
		return;
	main_emu->saveState("/ram/quick");
	/*
	if (main_emu->saveState("/ram/quick"))
		InGameMsg("Quick state saved");
	else
		InGameMsg("Could not save quick state");
	*/
}

void LoadQuickState()
{
	if (!main_emu)
		return;
	main_emu->loadState("/ram/quick");
	/*
	if (main_emu->loadState("/ram/quick"))
		InGameMsg("Quick state loaded");
	else
		InGameMsg("Could not load quick state");
	*/
}

void SaveState()
{
	vmu_dev = maple_enum_type(0, MAPLE_FUNC_MEMCARD);
	if (!main_emu || !vmu_dev)
		return;
	char temp[0x100];
	if (snap_save_to_vmu(vmu_dev, main_emu, g_save_slot))
	{
		sprintf(temp, "Saved state %d", g_save_slot);
		//InGameMsg(temp);
	}
	else
	{
		sprintf(temp, "Could not save state %d", g_save_slot);
		//InGameMsg(temp);
	}
}

void LoadState()
{
	vmu_dev = maple_enum_type(0, MAPLE_FUNC_MEMCARD);
	if (!main_emu || !vmu_dev)
		return;
	char temp[0x100];
	if (snap_load_from_vmu(vmu_dev, main_emu, g_save_slot))
	{
		sprintf(temp, "Loaded state %d", g_save_slot);
		//InGameMsg(temp);
	}
	else
	{
		sprintf(temp, "Could not load state %d", g_save_slot);
		//InGameMsg(temp);
	}
}

uint16 get_nesscreen_pixel_color(int x, int y)
{
	return nes_screen[y * 320 + x + 8];
}

void dc_nesscreen_copy_texture_320x240to256x256(pvr_ptr_t txr_addr)
{
	while (pvr_state.render_busy);
	int h;
	uint16 *d = (uint16*) (txr_addr);
	uint16 *s = nes_screen + 8;

	for (h = 0; h < 224; ++h)
	{
		sq_cpy(d, s, 256 * 2);
		d += 256;
		s += 320;
	}
}

void poll_input_optional_gun(NES *emu)
{
	cont_state_t* stat;
	emu->NES_gun.buttons = 0;
	stat = (cont_state_t*) maple_dev_status(dc_maple_controller_info->dev);
	if (stat->buttons & CONT_DPAD_UP)
		emu->NES_gun.y -= 2;
	if (stat->buttons & CONT_DPAD_DOWN)
		emu->NES_gun.y += 2;
	if (stat->buttons & CONT_DPAD_LEFT)
		emu->NES_gun.x -= 2;
	if (stat->buttons & CONT_DPAD_RIGHT)
		emu->NES_gun.x += 2;
	if (stat->buttons & CONT_X)
		emu->NES_gun.buttons |= NES_device_gun::BUTTON_L;
	if (stat->buttons & CONT_A)
		emu->NES_gun.buttons |= NES_device_gun::BUTTON_R;

	if (g_mice->dev)
	{
		DCMouse_Poll(g_mice);
		emu->NES_gun.x += g_mice->state.dx;
		emu->NES_gun.y += g_mice->state.dy;
		if (emu->NES_gun.y > 224)
			emu->NES_gun.y = 224;
		if (DCMouse_IsPressed(g_mice, MOUSE_LEFTBUTTON))
			emu->NES_gun.buttons |= NES_device_gun::BUTTON_L;
		if (DCMouse_IsPressed(g_mice, MOUSE_RIGHTBUTTON))
			emu->NES_gun.buttons |= NES_device_gun::BUTTON_R;
	}
	emu->NES_gun.normalize_position();
}


void poll_input_family_keyboard(NES *emu)
{
	emu->NES_kbd.clear_register();

	if (dc_maple_keyboard_info->dev)
	{
		kbd_state_t *kstat;
		kbd_cond_t *cond;
		uint32 i;
		kstat = (kbd_state_t*) maple_dev_status(dc_maple_keyboard_info->dev);
		cond = &(kstat->cond);
		if (cond->modifiers & KBD_MOD_LCTRL)
			emu->NES_kbd.setkey(NES_device_kbd::NES_KEY_Control_L);
		if (cond->modifiers & KBD_MOD_LSHIFT)
			emu->NES_kbd.setkey(NES_device_kbd::NES_KEY_Shift_L);
		if (cond->modifiers & KBD_MOD_RSHIFT)
			emu->NES_kbd.setkey(NES_device_kbd::NES_KEY_Shift_R);
		if (cond->modifiers & KBD_MOD_LALT)
			emu->NES_kbd.setkey(NES_device_kbd::NES_KEY_Grph);

		for (i = 0; i < sizeof(cond->keys) / sizeof(cond->keys[0]); ++i)
		{
			if (cond->keys[i])
				emu->NES_kbd.setkey(keymap_dc2nes[cond->keys[i]]);
		}
	}
}

void poll_input_cclimber(NES *emu)
{
	cont_state_t *stat;
	stat = (cont_state_t*) maple_dev_status(dc_maple_controller_info->dev);
	emu->NES_cclimber.left_UP = (stat->buttons & CONT_DPAD_UP) ? 1 : 0;
	emu->NES_cclimber.left_DOWN = (stat->buttons & CONT_DPAD_DOWN) ? 1 : 0;
	emu->NES_cclimber.left_LEFT = (stat->buttons & CONT_DPAD_LEFT) ? 1 : 0;
	emu->NES_cclimber.left_RIGHT = (stat->buttons & CONT_DPAD_RIGHT) ? 1 : 0;
	emu->NES_cclimber.right_UP = (stat->buttons & CONT_Y) ? 1 : 0;
	emu->NES_cclimber.right_DOWN = (stat->buttons & CONT_A) ? 1 : 0;
	emu->NES_cclimber.right_LEFT = (stat->buttons & CONT_X) ? 1 : 0;
	emu->NES_cclimber.right_RIGHT = (stat->buttons & CONT_B) ? 1 : 0;
}

void poll_input_paddle(NES *emu)
{
	mouse_state_t *mstat;
	emu->NES_paddle.buttons = 0;
	if (dc_maple_mouse_info->dev)
	{
		mstat = (mouse_state_t*) maple_dev_status (dc_maple_mouse_info->dev);
		emu->NES_paddle.x += mstat->dx;
		if (mstat->buttons & MOUSE_LEFTBUTTON)
			emu->NES_paddle.buttons |= NES_device_paddle::BUTTON_L;
		if (mstat->buttons & MOUSE_RIGHTBUTTON)
			emu->NES_paddle.buttons |= NES_device_paddle::BUTTON_R;
	}
	emu->NES_paddle.normalize_position();
}

void poll_input_mahjong (NES *emu)
{
	emu->NES_kbd.clear_register ();
	if (dc_maple_keyboard_info->dev)
	{
		kbd_state_t *kstat;
		kstat = (kbd_state_t *)maple_dev_status (dc_maple_keyboard_info->dev);
		emu->NES_mahjong.setkey(kstat->matrix);
	}
}

#define POLL_NES_B			1
#define POLL_NES_A			(1 << 1)
#define POLL_NES_SELECT		(1 << 2)
#define POLL_NES_START		(1 << 3)
#define POLL_NES_COIN		(1 << 4)
#define POLL_NES_AUTO_B		(1 << 5)
#define POLL_NES_AUTO_A		(1 << 6)

int poll_input(NES* emu)
{
	struct DCController* controller;
	NES_pad* pad;
	int k;
	for (k = 0; k < 4; ++k)
	{
		controller = g_controllers + k;
		if (!controller->dev)
			continue;
		DCController_Poll(controller);
		if (k == 0 && just_entered_game && (DCController_IsPressed(controller, CONT_A) || DCController_IsPressed(controller, CONT_B)))
			continue;
		just_entered_game = false;
		pad = emu->get_pad(k & 0x03);
		if (use_analog[k])
		{
			if (k == 0 && DCController_IsPressed(controller, CONT_DPAD_LEFT))
			{
				if (DCController_IsPressed(controller, CONT_START))
					return 1;
			}

			pad->nes_UP = (controller->state.joyy < -ANALOG_THRESHOLD);
			pad->nes_DOWN = (controller->state.joyy > ANALOG_THRESHOLD);
			pad->nes_LEFT = (controller->state.joyx < -ANALOG_THRESHOLD);
			pad->nes_RIGHT = (controller->state.joyx > ANALOG_THRESHOLD);
		}
		else
		{
			if (k == 0 && (controller->state.joyx < -ANALOG_THRESHOLD))
			{
				if (DCController_IsPressed(controller, CONT_START))
					return 1;
			}

			pad->nes_UP = DCController_IsPressed(controller, CONT_DPAD_UP);
			pad->nes_DOWN = DCController_IsPressed(controller, CONT_DPAD_DOWN);
			pad->nes_LEFT = DCController_IsPressed(controller, CONT_DPAD_LEFT);
			pad->nes_RIGHT = DCController_IsPressed(controller, CONT_DPAD_RIGHT);
		}

		uint32 loc_button_flags = controller->state.buttons;
		if (controller->state.ltrig > 128)
			loc_button_flags |= CONT_L;
		if (controller->state.rtrig > 128)
			loc_button_flags |= CONT_R;

		if ((loc_button_flags & g_button_map[k][CONTROLLER_QUICK_SAVE]) == g_button_map[k][CONTROLLER_QUICK_SAVE])
			SaveQuickState();
		if ((loc_button_flags & g_button_map[k][CONTROLLER_QUICK_LOAD]) == g_button_map[k][CONTROLLER_QUICK_LOAD])
			LoadQuickState();

		pad->nes_B = (loc_button_flags & g_button_map[k][CONTROLLER_NES_B]) == g_button_map[k][CONTROLLER_NES_B];
		pad->nes_A = (loc_button_flags & g_button_map[k][CONTROLLER_NES_A]) == g_button_map[k][CONTROLLER_NES_A];
		pad->nes_SELECT = (loc_button_flags & g_button_map[k][CONTROLLER_NES_SELECT]) == g_button_map[k][CONTROLLER_NES_SELECT];
		pad->nes_START = (loc_button_flags & g_button_map[k][CONTROLLER_NES_START]) == g_button_map[k][CONTROLLER_NES_START];
		emu->SetCoin((loc_button_flags & g_button_map[k][CONTROLLER_NES_COIN]) == g_button_map[k][CONTROLLER_NES_COIN]);

		if (!pad->nes_B)
		{
			if ((loc_button_flags & g_button_map[k][CONTROLLER_NES_B_AUTOFIRE]) == g_button_map[k][CONTROLLER_NES_B_AUTOFIRE])
				pad->nes_B = ((autofire_b[k]++ % autofire_b_speed[k]) == 0);
			else
				autofire_b[k] = 0;
		}

		if (!pad->nes_A)
		{
			if ((loc_button_flags & g_button_map[k][CONTROLLER_NES_A_AUTOFIRE]) == g_button_map[k][CONTROLLER_NES_A_AUTOFIRE])
				pad->nes_A = ((autofire_a[k]++ % autofire_a_speed[k]) == 0);
			else
				autofire_a[k] = 0;
		}
	}

	if (emu->ex_controller_type != NES::EX_NONE)
	{
		switch (emu->ex_controller_type)
		{
		case NES::EX_OPTICAL_GUN:
		case NES::EX_SPACE_SHADOW_GUN:
			poll_input_optional_gun(emu);
			break;

		case NES::EX_FAMILY_KEYBOARD:
			poll_input_family_keyboard(emu);
			break;

		case NES::EX_CRAZY_CLIMBER:
			poll_input_cclimber(emu);
			break;

		case NES::EX_ARKANOID_PADDLE:
			poll_input_paddle(emu);
			break;

		case NES::EX_MAHJONG:
			poll_input_mahjong(emu);
			break;
		}
	}
	return 0;
}

void dc_snesscreen_commit_dr(pvr_poly_hdr_t *poly, uint32 color /*= 0xffffffff*/)
{
	pvr_dr_state_t dr_state;
	pvr_vertex_t* vert;

	pvr_prim (poly, sizeof(pvr_poly_hdr_t));

	pvr_dr_init(dr_state);

	vert = pvr_dr_target(dr_state);
	vert->flags = PVR_CMD_VERTEX;
	vert->x = screen_adjustments.x1;
	vert->y = screen_adjustments.y2;
	vert->z = 10;
	vert->u = 0;
	vert->v = 224.0f/256.0f;
	vert->argb = color;
	vert->oargb = 0;
	pvr_dr_commit(vert);

	vert = pvr_dr_target(dr_state);
	vert->flags = PVR_CMD_VERTEX;
	vert->x = screen_adjustments.x1;
	vert->y = screen_adjustments.y1;
	vert->z = 10;
	vert->u = 0;
	vert->v = 0;
	vert->argb = color;
	vert->oargb = 0;
	pvr_dr_commit(vert);

	vert = pvr_dr_target(dr_state);
	vert->flags = PVR_CMD_VERTEX;
	vert->x = screen_adjustments.x2;
	vert->y = screen_adjustments.y2;
	vert->z = 10;
	vert->u = 1.0;
	vert->v = 224.0f/256.0f;
	vert->argb = color;
	vert->oargb = 0;
	pvr_dr_commit(vert);

	vert = pvr_dr_target(dr_state);
	vert->flags = PVR_CMD_VERTEX_EOL;
	vert->x = screen_adjustments.x2;
	vert->y = screen_adjustments.y1;
	vert->z = 10;
	vert->u = 1.0;
	vert->v = 0;
	vert->argb = color;
	vert->oargb = 0;
	pvr_dr_commit(vert);
}

static void dc_snesscreen_commit_texture(pvr_poly_hdr_t *poly, uint32 color /*= 0xffffffff*/)
{
	pvr_vertex_t vert;

	pvr_prim (poly, sizeof(pvr_poly_hdr_t));

	vert.flags = PVR_CMD_VERTEX;
	vert.x = screen_adjustments.x1;
	vert.y = screen_adjustments.y2;
	vert.z = 10;
	vert.u = 0;
	vert.v = 224.0f/256.0f;
	vert.argb = color;
	vert.oargb = 0;
	pvr_prim (&vert, sizeof(vert));

	vert.x = screen_adjustments.x1;
	vert.y = screen_adjustments.y1;
	vert.u = 0;
	vert.v = 0;
	pvr_prim (&vert, sizeof(vert));

	vert.x = screen_adjustments.x2;
	vert.y = screen_adjustments.y2;
	vert.u = 1.0;
	vert.v = 224.0f/256.0f;
	pvr_prim (&vert, sizeof(vert));

	vert.flags = PVR_CMD_VERTEX_EOL;
	vert.x = screen_adjustments.x2;
	vert.y = screen_adjustments.y1;
	vert.u = 1.0;
	vert.v = 0;
	pvr_prim (&vert, sizeof(vert));
}

void display_nes_screen_dr(NES* emu)
{
	text_counter++;
	if (text_counter == 2)
		text_counter = 0;
	dc_nesscreen_copy_texture_320x240to256x256(nes_texture_addr[text_counter]);
	pvr_wait_ready();
	pvr_scene_begin();
	pvr_list_begin(PVR_LIST_OP_POLY);
	if (bilinear_filtering)
		dc_snesscreen_commit_dr(&nes_pvr_poly_header_filter_bilinear[text_counter], 0xffffffff);
	else
		dc_snesscreen_commit_dr(&nes_pvr_poly_header_filter_none[text_counter], 0xffffffff);
	pvr_list_finish();
	pvr_scene_finish();
}

void display_nes_screen(NES* emu)
{
	dc_nesscreen_copy_texture_320x240to256x256(nes_texture_addr[text_counter]);
	pvr_wait_ready();
	pvr_scene_begin();
	pvr_list_begin(PVR_LIST_OP_POLY);
	if (bilinear_filtering)
		dc_snesscreen_commit_texture(&nes_pvr_poly_header_filter_bilinear[text_counter], 0xffffffff);
	else
		dc_snesscreen_commit_texture(&nes_pvr_poly_header_filter_none[text_counter], 0xffffffff);
	pvr_list_finish();
	pvr_scene_finish();
}

static volatile int rest_frames = 0;

static void timer_handler(irq_t source, irq_context_t *context)
{
	++rest_frames;
}

static void do_autoloop(NES *emu)
{
	timer_prime (TMU2, 60, true);
	irq_set_handler (EXC_TMU2_TUNI2, timer_handler);
	timer_start (TMU2);

	rest_frames = 1;
	for (;;)
	{
		emu->emulate_frame (nes_screen);
		poll_input (emu);

		display_nes_screen (emu);
		if (interrupt_game_loop) break;

		if (--rest_frames > 0)
		{
			int drop_frames = rest_frames;

			rest_frames = 0;
			while (drop_frames--)
			{
				emu->emulate_frame_skip ();
				poll_input (emu);
				if (interrupt_game_loop)
					break;
			}
		}
	}
	timer_stop (TMU2);
}

void do_loop(NES *emu)
{
	emu->thaw();
	interrupt_game_loop = false;

	for (;;)
	{
		poll_input(emu);
		if (interrupt_game_loop)
			break;
		emu->emulate_frame(nes_screen);
		display_nes_screen_dr(emu);
	}

	emu->freeze();
}

bool8 LoadROM(const char* fn)
{
	if (main_emu)
		delete main_emu;
	if (main_snd_mgr)
		delete main_snd_mgr;
	
	NES* temp_emu = NULL;
	dc_sound_mgr* temp_snd_mgr = NULL;
	null_sound_mgr nully;
	temp_emu = new NES(&nully);
	sprintf(temp_emu->disksys_rom_filename, "/cd/disksys.rom");
	if (!(temp_emu->initialize(fn)))
		goto finish;

	sram_load_from_vmu(vmu_dev, temp_emu);
	//disk_load_from_vmu(vmu_dev, emu);

	temp_snd_mgr = new dc_sound_mgr();
	if (!(temp_snd_mgr->initialize()))
		goto finish;
	temp_emu->new_snd_mgr(temp_snd_mgr);

	main_emu = temp_emu;
	main_snd_mgr = dc_main_snd_mgr = temp_snd_mgr;
	return TRUE;

finish:
	if (temp_emu)
		delete (temp_emu);
	if (temp_snd_mgr)
		delete (temp_snd_mgr);
	return FALSE;
}

void displayPreImage()
{
	Image img;
	Image_Init(&img);
	if (Image_Load(&img, "/cd/graphics/Splash01.jpg", PVR_LIST_OP_POLY, PVR_FILTER_NONE, IMG_ALPHA_NONE))
	{
		pvr_wait_ready();
		pvr_scene_begin();
		pvr_list_begin(PVR_LIST_OP_POLY);
		Image_Draw(&img, 0, 0, 1, ARGB_PACK_COLOR(255, 255, 255, 255), 100);
		pvr_list_finish();
		pvr_scene_finish();
		Image_Destroy(&img);
	}
}

extern "C" int main()
{	
	//video_discover();
	
	vid_init(DM_640x480, PM_RGB565);
	
	pvr_init(&params);

	texture_init();
	
	displayPreImage();

	DreamScript_Init();

	ExecState* exec = DreamScript_GetInterpreter()->globalExec();
	InitNesterObject(exec, *DreamScript_GetGlobalObject());
	InitNESDBClass(exec, *DreamScript_GetGlobalObject());

	vmu_dev = maple_enum_type(0, MAPLE_FUNC_MEMCARD);

	keymap_dc2nes_init();
	
	printf("test\n");

	DreamScript_EvaluateFile("/cd/main.js");

	while (true);

	return 0;
}

