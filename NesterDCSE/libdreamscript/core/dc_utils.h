#ifndef _DC_UTILS_H_
#define _DC_UTILS_H_

#define DC_SCREEN_WIDTH		640
#define DC_SCREEN_HEIGHT	480

#if defined(__cplusplus) || defined(c_plusplus)

#define EXTERN_C extern "C"
#define START_EXTERN_C extern "C" {
#define END_EXTERN_C }

#else

#define EXTERN_C extern
#define START_EXTERN_C
#define END_EXTERN_C
	
#endif

START_EXTERN_C

#define MIN_Y	16

#include <kos.h>
#include <plx/font.h>
#include "unzip.h"

#ifndef FALSE
#define FALSE 0
#endif

#ifndef TRUE
#define TRUE 1
#endif

extern const char *progname;

/* ------------------------------------------------------------ */
/* controller handling */

/* low level interface */
typedef enum {
  DC_MAPLE_CONTROLLER_NORMAL,
  DC_MAPLE_CONTROLLER_ARCADESTICK, 
} dc_maple_controller_type_t;

typedef enum {
  DC_MAPLE_MOUSE_3BUTTON,
} dc_maple_mouse_type_t;

typedef enum {
  DC_MAPLE_KEYBOARD_JP106,
} dc_maple_keyboard_type_t;

struct dc_maple_info_t
{
  maple_device_t *dev;
  
  union {
    dc_maple_controller_type_t c_type;
    dc_maple_mouse_type_t m_type;
    dc_maple_keyboard_type_t k_type;
  } type;
};

#define DC_MAPLE_INFO_SIZE 16
extern struct dc_maple_info_t dc_maple_controller_info[DC_MAPLE_INFO_SIZE];
extern struct dc_maple_info_t dc_maple_mouse_info[DC_MAPLE_INFO_SIZE];
extern struct dc_maple_info_t dc_maple_keyboard_info[DC_MAPLE_INFO_SIZE];
void dc_maple_init ();

struct dc_screen_offset_t
{
	float x, y;
};

extern struct dc_screen_offset_t dc_screen_offset;

extern pvr_vertex_t main_vert;

void dc_screen_offset_init();

void dc_pvr_init ();

maple_device_t* dc_vmu_search_file (const char* filename);
int load_bmp(uint16** raw, const char *filename, uint32* width, uint32* height, uint32 fmt/* = PVR_TXRFMT_RGB565*/);
void DrawString(const char* str, plx_fcxt_t* pCxt, point_t point, uint32 color);
void DrawStringWithBorder(const char* str, plx_fcxt_t* pCxt, point_t point, uint32 color, uint32 border_color);
uint8 BrowseForFile(const char* szStartingDir, char* szResult, plx_fcxt_t* font, int16 x, int16 y, int16 w, int16 h, int16 depth, cont_state_t* prevstate/* = NULL*/);
uint8 scherzo_bmp_load_texture(const char* filename, pvr_ptr_t* tex, uint32 *w, uint32 *h, uint32 *tex_width, uint32 *tex_height);
char* read_text_file(const char* filename);
uint8 hexstr_to_uint32(const char* str, uint32* retval);

typedef void (*dc_pvr_bgfunc_t)();

/* font */
void dc_pvr_font_commit_start();
void dc_pvr_font_commit_string(float x, float y, float z, uint32 argb, const char* str);
void dc_pvr_font_output_message(const char* title, const char* message, const char* foot, dc_pvr_bgfunc_t bgfunc);

/* diagram */
void dc_pvr_diagram_commit_start ();
void dc_pvr_diagram_commit_line(float x1, float y1, float x2, float y2, float z, uint32 argb);
void dc_pvr_diagram_commit_box (float x1, float y1, float x2, float y2, float z, uint32 argb);

void dc_wait_sq_cpy_done();

struct S_SCREEN_ADJUSTMENTS
{
	int32 x1, y1, x2, y2;
	float xscale, yscale;
};

extern struct S_SCREEN_ADJUSTMENTS screen_adjustments;

char get_largest_rom_name_in_zip_fp(unzFile file, char* rcvbuf);

extern uint32 badstuffcount;
#define REPORT_BAD_STUFF dbgio_printf("BAD STUFF in " __FILE__ " on line %d count = %d\n", __LINE__, badstuffcount++);

END_EXTERN_C

#endif

