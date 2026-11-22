#ifndef _MAIN_H_
#define _MAIN_H_

#include <kos.h>
#include "nes.h"
#include "dc_sound_mgr.h"

extern const char *progname;
extern uint8 keymap_jp106_dc2nes[256];
extern uint8* keymap_dc2nes;
extern uint16 nes_screen[320 * 240];
extern pvr_ptr_t nes_texture_addr[2];
extern pvr_poly_hdr_t nes_pvr_poly_header_filter_none[2];
extern pvr_poly_hdr_t nes_pvr_poly_header_filter_bilinear[2];
extern pvr_ptr_t dc_texture_addr;
extern pvr_poly_hdr_t dc_pvr_poly_header;
extern bool8 bilinear_filtering;
extern uint32 text_counter;
extern bool just_entered_game;
extern char rom_filename[0x100];
extern char rom_title[0x100];
extern NES* main_emu;
extern sound_mgr* main_snd_mgr;
extern dc_sound_mgr* dc_main_snd_mgr;
extern maple_device_t* vmu_dev;
enum
{
	CONTROLLER_NES_B = 0,
	CONTROLLER_NES_A,
	CONTROLLER_NES_SELECT,
	CONTROLLER_NES_START,
	CONTROLLER_NES_COIN,
	CONTROLLER_NES_B_AUTOFIRE,
	CONTROLLER_NES_A_AUTOFIRE,
	CONTROLLER_QUICK_SAVE,
	CONTROLLER_QUICK_LOAD,
	CONTROLLER_NES_COUNT
};

#define CONT_L			(CONT_DPAD2_RIGHT << 1)
#define CONT_R			(CONT_DPAD2_RIGHT << 2)
#define EMPTY_MAPPING	(CONT_DPAD2_RIGHT << 3)

extern uint32 g_button_map[4][CONTROLLER_NES_COUNT];
extern uint32 autofire_b[4];
extern uint32 autofire_a[4];
extern uint32 autofire_a_speed[4];
extern uint32 autofire_b_speed[4];
extern bool8 interrupt_game_loop;
extern bool8 use_analog[4];

extern int g_save_slot;

bool8 LoadROM(const char* fn);
void dc_snesscreen_commit_dr(pvr_poly_hdr_t *poly, uint32 color /*= 0xffffffff*/);
void dc_nesscreen_copy_texture_320x240to256x256(pvr_ptr_t txr_addr);
int poll_input(NES* emu);
void SaveState();
void LoadState();

#endif
