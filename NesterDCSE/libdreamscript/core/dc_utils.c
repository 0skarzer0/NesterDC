#include <kos.h>
#include <stdio.h>
#include "dc_utils.h"
#include "dc_vmu.h"
#include <math.h>
#include <stdlib.h>
#include "unzip.h"

struct S_SCREEN_ADJUSTMENTS screen_adjustments = {0, 0, DC_SCREEN_WIDTH, DC_SCREEN_HEIGHT, 1.0f, 1.0f};

#include <imageload/jitterdefs.h>
//#include "jitter_table.h"

/* ============================================================ */
/* sq */

#include <dc/sq.h>

/* ============================================================ */
/* controller handling */
struct dc_maple_info_t dc_maple_controller_info[DC_MAPLE_INFO_SIZE];
struct dc_maple_info_t dc_maple_mouse_info[DC_MAPLE_INFO_SIZE];
struct dc_maple_info_t dc_maple_keyboard_info[DC_MAPLE_INFO_SIZE];

uint32 badstuffcount = 0;

void dc_maple_init ()
{
	int n;
	maple_device_t *dev;
	struct dc_maple_info_t *p;

	struct _typemap
	{
		char *name;
		dc_maple_controller_type_t type;
	} typemap[] = {
		{"Arcade Stick", DC_MAPLE_CONTROLLER_ARCADESTICK},
		{NULL, DC_MAPLE_CONTROLLER_NORMAL},
	};
	struct _typemap *tp;
	
	memset(&dc_maple_controller_info, 0, sizeof(dc_maple_controller_info));
	memset(&dc_maple_mouse_info, 0, sizeof(dc_maple_controller_info));
	memset(&dc_maple_keyboard_info, 0, sizeof(dc_maple_controller_info));

	// controller
	n = 0;
	p = dc_maple_controller_info;
	for (;;)
	{
		dev = maple_enum_type (n, MAPLE_FUNC_CONTROLLER);
		p->dev = dev;
		p->type.c_type = DC_MAPLE_CONTROLLER_NORMAL;

		tp = typemap;
		while (tp->name)
		{
			if (!strncmp(tp->name, dev->info.product_name, strlen(tp->name)))
			{
				p->type.c_type = tp->type;
				break;
			}

			++tp;
		}

		++n;
		++p;

		if (!dev)
			break;
	}

	// mouse
	n = 0;
	p = dc_maple_mouse_info;
	for (;;)
	{
		dev = maple_enum_type (n, MAPLE_FUNC_MOUSE);
		p->dev = dev;
		p->type.m_type = DC_MAPLE_MOUSE_3BUTTON;

		++n;
		++p;

		if (!dev)
			break;
	}

	// keyboard
	n = 0;
	p = dc_maple_keyboard_info;
	for (;;)
	{
		dev = maple_enum_type (n, MAPLE_FUNC_KEYBOARD);
		p->dev = dev;
		p->type.k_type = DC_MAPLE_KEYBOARD_JP106;

		++n;
		++p;

		if (!dev)
			break;
	}
}


/* ------------------------------------------------------------ */
/* screen rect */

struct dc_screen_offset_t dc_screen_offset = {0, 16};

void dc_screen_offset_init ()
{
  dc_screen_offset.x = 0;
  dc_screen_offset.y = 16;
}

maple_device_t* dc_vmu_search_file (const char* filename)
{
	int n;
	maple_device_t* dev;
	n = 0;
	for (;;)
	{
		dev = maple_enum_type (n, MAPLE_FUNC_MEMCARD);
		if (!dev)
			return NULL;
		if (ndc_vmu_get_dirent (NULL, dev, filename) >= 0)
			return dev;
		++n;
	}
}

int load_bmp (uint16** pRaw, const char *filename, uint32* width, uint32* height, uint32 fmt)
{
	FILE *fp = NULL;
	uint8* bmp = NULL;
	uint8 *p;
	uint16* raw = NULL;
	uint32 i;
	uint32 area;
	uint32 bmp_size;

	fp = fopen(filename, "rb");
	if (!fp)
		goto error;
	
	fseek(fp, 18, SEEK_SET);
	fread(width, sizeof(uint32), 1, fp);
	fread(height, sizeof(uint32), 1, fp);
	
	area = (*width) * (*height);
	bmp_size = area * 3;
	bmp = (uint8*) malloc(bmp_size);
	//raw = *pRaw = new uint16[area];
	raw = *pRaw = (uint16*) memalign(32, bmp_size);

	fseek(fp, 54, SEEK_SET);
	if (fread(bmp, bmp_size, 1, fp) != 1) 
		goto error;

	fclose(fp);

	p = bmp;
	uint16 r, g, b;
	
	for (i = 0; i < area; ++i)
	{
		b = *p++ * 32 / 256;
		g = *p++ * 64 / 256;
		r = *p++ * 32 / 256;

		*raw++ = (r << 11) | (g << 5) | (b << 0);
	}
	
	if (bmp)
		free(bmp);

	return 0;

error:
	if (fp)
 		fclose(fp);
	if (bmp)
		free(bmp);
	if (raw)
		free(raw);
	*pRaw = NULL;
	return -1;
}

void
dc_pvr_init ()
{ 
  dc_screen_offset_init ();
}

void DrawString(const char* str, plx_fcxt_t* pCxt, point_t point, uint32 color)
{
	float old_font_size = pCxt->size;
	pCxt->size = pCxt->size * screen_adjustments.yscale;
	
	char tempstr[0x200];
	strcpy(tempstr, str);
	
	char* curstr = tempstr;
	
	char* tabstart = NULL;
	char* tabend = NULL;
	uint32 tablen = 0;
	
	while (TRUE)
	{
		point_t working_point = point;
		
		working_point.x += tablen;
		
		tabstart = strstr(curstr, "[TAB]");
		if (tabstart)
		{
			tabend = strstr(curstr, "[/TAB]");
			if (!tabend)
				break;
			*tabstart = '\0';
			tabstart += 5;
			*tabend = '\0';
			tabend += 6;
			tablen = atoi(tabstart);
		}
		
		working_point.x = screen_adjustments.x1 + (working_point.x * screen_adjustments.xscale);
		working_point.y = screen_adjustments.y1 + (working_point.y * screen_adjustments.yscale);

		pCxt->color = color;
		plx_fcxt_begin(pCxt);
		plx_fcxt_setpos_pnt(pCxt, &working_point);
		plx_fcxt_draw(pCxt, curstr);
		plx_fcxt_end(pCxt);
		
		if (tabstart)
			curstr = tabend;
		else
			break;
	}

	pCxt->size = old_font_size;
}

void DrawStringWithBorder(const char* str, plx_fcxt_t* pCxt, point_t point, uint32 color, uint32 border_color)
{
	float old_font_size = pCxt->size;
	pCxt->size = pCxt->size * screen_adjustments.yscale;
	
	char tempstr[0x200];
	strcpy(tempstr, str);
	
	char* curstr = tempstr;
	
	char* tabstart = NULL;
	char* tabend = NULL;
	uint32 tablen = 0;
	
	while (TRUE)
	{
		point_t working_point = point;
		
		working_point.x += tablen;
		
		tabstart = strstr(curstr, "[TAB]");
		if (tabstart)
		{
			tabend = strstr(curstr, "[/TAB]");
			if (!tabend)
				break;
			*tabstart = '\0';
			tabstart += 5;
			*tabend = '\0';
			tabend += 6;
			tablen = atoi(tabstart);
		}
		
		working_point.x = screen_adjustments.x1 + (working_point.x * screen_adjustments.xscale);
		working_point.y = screen_adjustments.y1 + (working_point.y * screen_adjustments.yscale);

		pCxt->color = color;
		plx_fcxt_begin(pCxt);
		plx_fcxt_setpos_pnt(pCxt, &working_point);
		plx_fcxt_draw(pCxt, curstr);
		plx_fcxt_end(pCxt);

		--working_point.z;
		--working_point.x;
		--working_point.y;
		pCxt->color = border_color;
		plx_fcxt_begin(pCxt);
		plx_fcxt_setpos_pnt(pCxt, &working_point);
		plx_fcxt_draw(pCxt, curstr);
		plx_fcxt_end(pCxt);

		working_point.x += 2;
		plx_fcxt_begin(pCxt);
		plx_fcxt_setpos_pnt(pCxt, &working_point);
		plx_fcxt_draw(pCxt, curstr);
		plx_fcxt_end(pCxt);

		working_point.y += 2;
		plx_fcxt_begin(pCxt);
		plx_fcxt_setpos_pnt(pCxt, &working_point);
		plx_fcxt_draw(pCxt, curstr);
		plx_fcxt_end(pCxt);

		working_point.x -= 2;
		plx_fcxt_begin(pCxt);
		plx_fcxt_setpos_pnt(pCxt, &working_point);
		plx_fcxt_draw(pCxt, curstr);
		plx_fcxt_end(pCxt);
		
		if (tabstart)
			curstr = tabend;
		else
			break;
	}

	pCxt->size = old_font_size;
}

char* read_text_file(const char* filename)
{
	FILE* fp = fopen(filename, "rb");
	if (!fp)
		return NULL;
	fseek(fp, 0, SEEK_END);
	uint32 filelen = ftell(fp);
	fseek(fp, 0, SEEK_SET);
	char* sztemp = (char*) malloc(filelen + 1);
	fread(sztemp, sizeof(char), filelen, fp);
	sztemp[filelen] = '\0';
	fclose(fp);
	return sztemp;
}

uint8 hexstr_to_uint32(const char* str, uint32* retval)
{
	uint32 num = 0;
	int32 len = strlen(str);
	if (len == 0)
		return 0;
	uint32 digit = 0;
	uint32 temppow;
	int32 k;
	for (k = len - 1; k >= 0; --k)
	{
		temppow = pow(16, digit);
		switch (str[k])
		{
		case '0':
			break;
		case '1':
			num += temppow;
			break;
		case '2':
			num += temppow * 2;
			break;
		case '3':
			num += temppow * 3;
			break;
		case '4':
			num += temppow * 4;
			break;
		case '5':
			num += temppow * 5;
			break;
		case '6':
			num += temppow * 6;
			break;
		case '7':
			num += temppow * 7;
			break;
		case '8':
			num += temppow * 8;
			break;
		case '9':
			num += temppow * 9;
			break;
		case 'a':
		case 'A':
			num += temppow * 10;
			break;
		case 'b':
		case 'B':
			num += temppow * 11;
			break;
		case 'c':
		case 'C':
			num += temppow * 12;
			break;
		case 'd':
		case 'D':
			num += temppow * 13;
			break;
		case 'e':
		case 'E':
			num += temppow * 14;
			break;
		case 'f':
		case 'F':
			num += temppow * 15;
			break;
		default:
		{
			dbgio_printf("hexstr_to_uint32() bad char: %c\n", str[k]);
			return 0;
		}
		};
		++digit;
	}
	*retval = num;
	return 1;
}

static pvr_poly_hdr_t dc_pvr_font_poly_hdr;

/*
  load 12x24 font. (use as 14x26).
*/

#define DC_PVR_FONT_WIDTH 14
#define DC_PVR_FONT_HEIGHT 26


void dc_pvr_font_init ()
{
  pvr_ptr_t txr_font;
  pvr_poly_cxt_t poly;
  int x, y;
  int ch;
  uint16 font_image[256 * 256] __attribute__ ((aligned (32)));;
  
  /* setup font texture */
  txr_font = pvr_mem_malloc (256 * 256 * 2);
  pvr_poly_cxt_txr (&poly, PVR_LIST_TR_POLY,
                    PVR_TXRFMT_ARGB4444 | PVR_TXRFMT_NONTWIDDLED,
                    256, 256, txr_font, PVR_FILTER_NONE);
  pvr_poly_compile (&dc_pvr_font_poly_hdr, &poly);
  
  /* draw font to texture */
  memset(font_image, 0, sizeof(font_image));
  ch = 0;
  for (y = 0; y < 6; ++y)
  {
    for (x = 0; x < 16; ++x)
    {
      uint16 *p;
      
      p = font_image + 256 * y * DC_PVR_FONT_HEIGHT + x * DC_PVR_FONT_WIDTH;
      bfont_draw_thin (p, 256, 0, 32 + ch, 0);
      bfont_draw_thin (p + 1, 256, 0, 32 + ch, 0);
      
      ++ch;
    }
  }
  
  sq_cpy (txr_font, font_image, sizeof(font_image));
}


void dc_pvr_font_commit_start ()
{
  pvr_prim (&dc_pvr_font_poly_hdr, sizeof(pvr_poly_hdr_t));
}


static void dc_pvr_font_commit_char (float x, float y, float z, uint32 argb, char ch)
{
  pvr_vertex_t vert;
  int cx, cy;
  float u1, v1, u2, v2;
  
  cx = ((ch - 32) & 15) * DC_PVR_FONT_WIDTH;
  cy = ((ch - 32) >> 4) * DC_PVR_FONT_HEIGHT;
  u1 = cx * 1.0 / 256;
  v1 = cy * 1.0 / 256;
  u2 = (cx + DC_PVR_FONT_WIDTH) * 1.0 / 256;
  v2 = (cy + DC_PVR_FONT_HEIGHT) * 1.0 / 256;
  
  vert.flags = PVR_CMD_VERTEX;
  vert.x = dc_screen_offset.x + x;
  vert.y = dc_screen_offset.y + y + DC_PVR_FONT_HEIGHT;
  vert.z = z;
  vert.u = u1;
  vert.v = v2;
  vert.argb = argb;
  vert.oargb = 0;
  pvr_prim (&vert, sizeof(vert));
  
  vert.x = dc_screen_offset.x + x;
  vert.y = dc_screen_offset.y + y;
  vert.u = u1;
  vert.v = v1;
  pvr_prim (&vert, sizeof(vert));
  
  vert.x = dc_screen_offset.x + x + DC_PVR_FONT_WIDTH;
  vert.y = dc_screen_offset.y + y + DC_PVR_FONT_HEIGHT;
  vert.u = u2;
  vert.v = v2;
  pvr_prim (&vert, sizeof(vert));
  
  vert.flags = PVR_CMD_VERTEX_EOL;
  vert.x = dc_screen_offset.x + x + DC_PVR_FONT_WIDTH;
  vert.y = dc_screen_offset.y + y;
  vert.u = u2;
  vert.v = v1;
  pvr_prim (&vert, sizeof(vert));
}


void dc_pvr_font_commit_string (float x, float y, float z, uint32 argb, const char *str)
{
  if (!str) return;
  
  while (*str)
  {
    dc_pvr_font_commit_char (x, y, z, argb, *str);
    
    x += 12;
    ++str;
  }
}

void dc_pvr_font_output_message (const char *title, const char *message, const char *foot, dc_pvr_bgfunc_t bgfunc)
{
  int i;
  int y;
  uint32 f_color = 0xffffffff;
  uint32 b_color = 0xffffa500;
  uint32 w_color = 0xd0010101;
  
  /* fill PVR cache */
  for (i = 0; i < 2; ++i)
  {
    pvr_wait_ready ();
    pvr_scene_begin ();
    
    pvr_list_begin (PVR_LIST_OP_POLY);
    if (bgfunc) bgfunc();
    pvr_list_finish ();
    
    pvr_list_begin (PVR_LIST_TR_POLY);
    dc_pvr_font_commit_start ();
    if (title)
    {
      dc_pvr_font_commit_string (20, 50, 512, f_color, title);
      dc_pvr_font_commit_string (21, 51, 510, b_color, title);
    }
    
    if (message)
      dc_pvr_font_commit_string (50, 150, 512, f_color, message);
    
    if (foot) 
    {
      dc_pvr_font_commit_string (20, 400, 512, f_color, foot);
      dc_pvr_font_commit_string (21, 401, 510, b_color, foot);
    }
    
    dc_pvr_diagram_commit_start ();
    y = 80;
    dc_pvr_diagram_commit_line (20, y, 600, y, 500, b_color); 
    y = 430;
    dc_pvr_diagram_commit_line (20, y, 600, y, 500, b_color); 
    
    dc_pvr_diagram_commit_box (0, 0, 640, 480, 300, w_color);
    
    pvr_list_finish ();
    pvr_scene_finish ();
  }
}

/* ------------------------------------------------------------ */
pvr_poly_hdr_t dc_pvr_diagram_poly_hdr;

void dc_pvr_diagram_init()
{
  pvr_poly_cxt_t poly;
  
  pvr_poly_cxt_col (&poly, PVR_LIST_TR_POLY);
  pvr_poly_compile(&dc_pvr_diagram_poly_hdr, &poly);
}


void dc_pvr_diagram_commit_start ()
{
  pvr_prim(&dc_pvr_diagram_poly_hdr, sizeof(pvr_poly_hdr_t));
}


void dc_pvr_diagram_commit_line(float x1, float y1, float x2, float y2, float z, uint32 argb)
{
  pvr_vertex_t vert;
  
  vert.flags = PVR_CMD_VERTEX;
  vert.x = dc_screen_offset.x + x1;
  vert.y = dc_screen_offset.y + y1 + 2;
  vert.z = z;
  vert.u = vert.v = 0.0f;
  vert.argb = argb;
  vert.oargb = 0;
  pvr_prim (&vert, sizeof(vert));
  
  vert.x = dc_screen_offset.x + x1;
  vert.y = dc_screen_offset.y + y1;
  pvr_prim (&vert, sizeof(vert));

  vert.x = dc_screen_offset.x + x2;
  vert.y = dc_screen_offset.y + y2 + 2;
  pvr_prim (&vert, sizeof(vert));
  
  vert.flags = PVR_CMD_VERTEX_EOL;
  vert.x = dc_screen_offset.x + x2;
  vert.y = dc_screen_offset.y + y2;
  pvr_prim(&vert, sizeof(vert));
}


void dc_pvr_diagram_commit_box (float x1, float y1, float x2, float y2, float z, uint32 argb)
{
  pvr_vertex_t vert;
  
  vert.flags = PVR_CMD_VERTEX;
  vert.x = dc_screen_offset.x + x1;
  vert.y = dc_screen_offset.y + y2;
  vert.z = z;
  vert.u = vert.v = 0.0f;
  vert.argb = argb;
  vert.oargb = 0;
  pvr_prim (&vert, sizeof(vert));
  
  vert.x = dc_screen_offset.x + x1;
  vert.y = dc_screen_offset.y + y1;
  pvr_prim (&vert, sizeof(vert));
  
  vert.x = dc_screen_offset.x + x2;
  vert.y = dc_screen_offset.y + y2;
  pvr_prim (&vert, sizeof(vert));
  
  vert.flags = PVR_CMD_VERTEX_EOL;
  vert.x = dc_screen_offset.x + x2;
  vert.y = dc_screen_offset.y + y1;
  pvr_prim(&vert, sizeof(vert));
}

void dc_wait_sq_cpy_done()
{
	/* wait for both store queues to complete */
	*((uint32 *)(0xe0000000)) = 0;
	*((uint32 *)(0xe0000020)) = 0;
}

char get_largest_rom_name_in_zip_fp(unzFile file, char* rcvbuf)
{
	char name[132];
	int port;
	unz_file_info info;
	int filesize = 0;
	int len;

	port = unzGoToFirstFile(file);
	while (port == UNZ_OK)
	{
		unzGetCurrentFileInfo(file, &info, name, 128, NULL, 0, NULL, 0);
		len = strlen(name);
		if (info.uncompressed_size > filesize && ((name[len - 4] == '.') && ((name[len - 3] == 'n') || (name[len - 3] == 'N')) && ((name[len - 2] == 'e') || (name[len - 2] == 'E')) && ((name[len - 1] == 's') || (name[len - 1] == 'S'))))
		{
			strcpy(rcvbuf, name);
			filesize = info.uncompressed_size;
		}
		port = unzGoToNextFile(file);
	}
	if (filesize == 0)
		return 0;
	return 1;
}

