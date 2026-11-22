#include "image.h"
#include <jpeg/jpeglib.h>
#include <math.h>
#include "dc_utils.h"

#include "jitter_table.h"

// size of n*n poly
uint32 POLYWH=64;

uint32 h=0,y=0,n=0,m=0,polysdeep=0;

#define UVDIFF	0.007

#define UVSTART	UVDIFF
#define UVEND	(1.0f - UVDIFF)

void scan_to_image_16(uint16* buffer, uint32 row_width, Image* img)
{ 
	uint16 ourbuffer[10000]={0};
    uint32 i;
    uint32 polysacross = 0;
    
    memcpy(ourbuffer, buffer, row_width * 2);
	
	if(n == POLYWH )
	{
		polysdeep += img->horiz_polys;
		n=0;
	}

    for(i = 0; i < img->horiz_polys; i++)
    {    	 
    	memcpy(((uint16*) img->polys[polysdeep+polysacross].ptr) + (y%POLYWH)*POLYWH, &ourbuffer[polysacross*POLYWH], 2*POLYWH);
    	polysacross++;
    }
    
	n++;
	y++; 
}

void scan_to_image_jpeg(char* buffer, uint32 row_width, Image* img)
{ 
	uint16 ourbuffer[10000]={0};
    uint32 i;
    uint32 polysacross = 0;
    
    
	for(i=0; i<row_width/3; i++){
		ourbuffer[i] = ((buffer[i*3]>>3)<<11) + ((buffer[i*3+1]>>2)<<5) + (buffer[i*3+2]>>3);
	}
    //memcpy(ourbuffer, buffer, row_width * 2);
	
	if(n == POLYWH )
	{
		polysdeep += img->horiz_polys;
		n=0;
	}

    for(i = 0; i < img->horiz_polys; i++)
    {    	 
    	memcpy(((uint16*) img->polys[polysdeep+polysacross].ptr) + (y%POLYWH)*POLYWH, &ourbuffer[polysacross*POLYWH], 2*POLYWH);
    	polysacross++;
    }
    
	n++;
	y++; 
}

int Image_Load_JPG(Image* img, const char* filename, pvr_list_t list, int filtering)
{
	img->list = list;
	
	pvr_poly_cxt_t poly;
	
    struct jpeg_decompress_struct cinfo;
    
    struct jpeg_error_mgr jerr;
    uint32 i,k,l,x;
    
	FILE * infile;
	JSAMPARRAY buffer;
	uint32 row_stride;

	if ((infile = fopen(filename, "rb")) == NULL)
		return 0;
    h=y=n=m=polysdeep=0;
    
    cinfo.err = jpeg_std_error(&jerr);
	   
	jpeg_create_decompress(&cinfo);

	jpeg_stdio_src(&cinfo, infile);

	(void) jpeg_read_header(&cinfo, TRUE);
	
	cinfo.scale_denom = 1;
	  
    (void) jpeg_start_decompress(&cinfo);
    
   	img->width = cinfo.output_width;
	img->height = cinfo.output_height;
    
    row_stride = cinfo.output_width * cinfo.output_components;
	  
	buffer = (*cinfo.mem->alloc_sarray)
	((j_common_ptr) &cinfo, JPOOL_IMAGE, row_stride, 1);

	y=h=i=0;

	while(i<row_stride/3)
		i+=POLYWH;
	img->horiz_polys=i/POLYWH;

	i=0;
	while(i<cinfo.output_height)
		i+=POLYWH;
    img->vert_polys=i/POLYWH;

	for(i = 0; i < img->vert_polys; i++)
	{
		for(k = 0; k < img->horiz_polys; k++)
		{
			if(y>POLYWH+POLYWH*h)
				h++;
			s_image_poly* temp = img->polys + (i*img->horiz_polys+k);
			temp->ptr = pvr_mem_malloc(POLYWH*POLYWH*2);
			pvr_poly_cxt_txr(&poly, list, PVR_TXRFMT_RGB565 | PVR_TXRFMT_NONTWIDDLED, POLYWH, POLYWH, temp->ptr, filtering);
			pvr_poly_compile(&(temp->hdr), &poly);
			for(x = 0; x<POLYWH*POLYWH; x++)
				((uint16*) temp->ptr)[x] = 0;
			
			temp->hasendu = FALSE;
			if (k == (img->horiz_polys - 1))
			{
				uint32 leftover = img->width % POLYWH;
				if (leftover == 0)
					temp->endu = UVEND;
				else
				{
					temp->endu = (((float)leftover) / ((float)POLYWH)) - UVDIFF;
					temp->hasendu = TRUE;
				}
			}
			else
				temp->endu = UVEND;

			temp->hasendv = FALSE;
			if (i == (img->vert_polys - 1))
			{
				uint32 leftover = img->height % POLYWH;
				if (leftover == 0)
					temp->endv = UVEND;
				else
				{
					temp->endv = (((float)leftover) / ((float)POLYWH)) - UVDIFF;
					temp->hasendv = TRUE;
				}
			}
			else
				temp->endv = UVEND;
		}
	}
    
    l=0;
    
    while (cinfo.output_scanline < cinfo.output_height)
    {
		(void) jpeg_read_scanlines(&cinfo, buffer, 1);
    	scan_to_image_jpeg(buffer[0], row_stride, img);
	}
    
    (void) jpeg_finish_decompress(&cinfo);
   
    jpeg_destroy_decompress(&cinfo);
   
    fclose(infile);
	
	return 1;
}

int Image_Load(Image* img, const char* filename, pvr_list_t list, int filtering, IMG_ALPHA alpha)
{
	if (stricmp(filename + (strlen(filename)-4), ".jpg")==0)
		return Image_Load_JPG(img, filename, list, filtering);
	uint32 rowbytes;
	uint32 i,k,l,x;
	pvr_poly_cxt_t poly;
	uint16* buf;
	img->list = list;
	
	h = y = n = m = polysdeep = 0;
	y = h = i = 0;
	
	IMG_INFO info = IMG_INFO_DEFAULTS;
	info.alpha = alpha;
	info.key = 0x00ff00ff;
	kos_img_t kimg;
	kimg.data = NULL;
	img_load_file(filename, &info, &kimg);
	if (!kimg.data)
		return 0;
	
	img->width = kimg.w;
	img->height = kimg.h;
	rowbytes = kimg.w * 2;
	buf = kimg.data;

	while(i < kimg.w)
		i += POLYWH;

	img->horiz_polys = i / POLYWH;

	i = 0;
	while(i < kimg.h)
		i += POLYWH;

	img->vert_polys = i / POLYWH;
	
	uint32 texfmt;
	switch (alpha)
	{
		case IMG_ALPHA_MASK:
			texfmt = PVR_TXRFMT_ARGB1555;
			break;
		case IMG_ALPHA_KEYED:
			texfmt = PVR_TXRFMT_ARGB1555;
			break;
		case IMG_ALPHA_FULL:
			texfmt = PVR_TXRFMT_ARGB4444;
			break;
		case IMG_ALPHA_NONE:
		default:
			texfmt = PVR_TXRFMT_RGB565;
			break;
	};

	for(i = 0; i < img->vert_polys; i++)
	{
		for(k = 0; k < img->horiz_polys; k++)
		{
			if(y>POLYWH+POLYWH*h)
				h++;
			s_image_poly* temp = img->polys + (i*img->horiz_polys+k);
			temp->ptr = pvr_mem_malloc(POLYWH*POLYWH*2);
			pvr_poly_cxt_txr(&poly, list, texfmt | PVR_TXRFMT_NONTWIDDLED, POLYWH, POLYWH, temp->ptr, filtering);
			pvr_poly_compile(&(temp->hdr), &poly);
			for(x = 0; x<POLYWH*POLYWH; x++)
				((uint16*) temp->ptr)[x] = 0;
			
			temp->hasendu = FALSE;
			if (k == (img->horiz_polys - 1))
			{
				uint32 leftover = img->width % POLYWH;
				if (leftover == 0)
					temp->endu = UVEND;
				else
				{
					temp->endu = (((float)leftover) / ((float)POLYWH)) - UVDIFF;
					temp->hasendu = TRUE;
				}
			}
			else
				temp->endu = UVEND;

			temp->hasendv = FALSE;
			if (i == (img->vert_polys - 1))
			{
				uint32 leftover = img->height % POLYWH;
				if (leftover == 0)
					temp->endv = UVEND;
				else
				{
					temp->endv = (((float)leftover) / ((float)POLYWH)) - UVDIFF;
					temp->hasendv = TRUE;
				}
			}
			else
				temp->endv = UVEND;
		}
	}

	for (l = 0; l < kimg.h; ++l)
		scan_to_image_16(buf + (kimg.w * l), kimg.w, img);
	
	kos_img_free(&kimg, 0);
	
	return 1;
}

void Image_Init(Image* img)
{
	int k = 0;
	for (; k < 256; ++k)
		img->polys[k].ptr = NULL;
	
	img->horiz_polys = 0;
	img->vert_polys = 0;
	img->width = 0;
	img->height = 0;
}

void Image_Destroy(Image* img)
{
	int k = 0;
	for (; k < (img->horiz_polys * img->vert_polys); ++k)
	{
		if (img->polys[k].ptr)
			pvr_mem_free(img->polys[k].ptr);
	}
}

void draw_poly(float x, float y, float xwidth, float yheight, s_image_poly* pol, uint32 color, float z)
{
	pvr_vertex_t v;
	
	pvr_prim(&pol->hdr, sizeof(pvr_poly_hdr_t));
	
	if (pol->hasendu)
		xwidth *= (pol->endu + UVDIFF);
	if (pol->hasendv)
		yheight *= (pol->endv + UVDIFF);
	
	float x2 = x + xwidth;
	float y2 = y + yheight;

	v.flags = PVR_CMD_VERTEX;
	v.x = x;
	v.y = y2;
	v.z = z;
	v.u = UVSTART;
	v.v = pol->endv;
	v.argb = color;
	v.oargb = 0;
	pvr_prim(&v, sizeof(v));

	v.x = x;
	v.y = y;
	v.u = UVSTART;
	v.v = UVSTART;
	pvr_prim(&v, sizeof(v));

	v.x = x2;
	v.y = y2;
	v.u = pol->endu;
	v.v = pol->endv;
	pvr_prim(&v, sizeof(v));

	v.flags = PVR_CMD_VERTEX_EOL;
	v.x = x2;
	v.y = y;
	v.u = pol->endu;
	v.v = UVSTART;
	pvr_prim(&v, sizeof(v));
}

void draw_poly4(pvr_vertex_t* verts, s_image_poly* pol, uint32 color, float z)
{
	verts[0].u = UVSTART;
	verts[0].v = pol->endv;
	verts[1].u = UVSTART;
	verts[1].v = UVSTART;
	verts[2].u = pol->endu;
	verts[2].v = pol->endv;
	verts[3].u = pol->endu;
	verts[3].v = UVSTART;
	
	verts[0].z = verts[1].z = verts[2].z = verts[3].z = z;
	
	verts[0].argb = verts[1].argb = verts[2].argb = verts[3].argb = color;
	verts[0].oargb = verts[1].oargb = verts[2].oargb = verts[3].oargb = 0;
	
	verts[0].flags = PVR_CMD_VERTEX;
	verts[1].flags = PVR_CMD_VERTEX;
	verts[2].flags = PVR_CMD_VERTEX;
	verts[3].flags = PVR_CMD_VERTEX_EOL;
	
	pvr_prim(&pol->hdr, sizeof(pvr_poly_hdr_t));
	pvr_prim(&verts[0], sizeof(pvr_vertex_t));
	pvr_prim(&verts[1], sizeof(pvr_vertex_t));
	pvr_prim(&verts[2], sizeof(pvr_vertex_t));
	pvr_prim(&verts[3], sizeof(pvr_vertex_t));
}

void Image_Draw(Image *img, float x, float y, float scale, uint32 color, float z)
{
   	uint32 i,k;
   	
   	x *= screen_adjustments.xscale;
   	y *= screen_adjustments.yscale;
   	
   	x += screen_adjustments.x1;
	y += screen_adjustments.y1;
   	
   	float cury = y;
   	float xstep = (((float)POLYWH) * scale) * screen_adjustments.xscale;
	float ystep = (((float)POLYWH) * scale) * screen_adjustments.yscale;
	
   	for(i = 0; i < img->vert_polys; ++i)
   	{
		float curx = x;
		for(k = 0; k < img->horiz_polys; ++k)
		{
			draw_poly(curx, cury, xstep, ystep, &img->polys[i*img->horiz_polys+k], color, z);
			curx += xstep;
		}
		cury += ystep;
	}
}

void Image_DrawTrue(Image *img, float x, float y, float scale, uint32 color, float z)
{
   	uint32 i,k;
   	
   	float cury = y;
   	float xstep = (((float)POLYWH) * scale);
	float ystep = (((float)POLYWH) * scale);
	
   	for(i = 0; i < img->vert_polys; ++i)
   	{
		float curx = x;
		for(k = 0; k < img->horiz_polys; ++k)
		{
			draw_poly(curx, cury, xstep, ystep, &img->polys[i*img->horiz_polys+k], color, z);
			curx += xstep;
		}
		cury += ystep;
	}
}

void Image_DrawWH(Image* img, float x, float y, float width, float height, uint32 color, float z)
{
	uint32 i,k;
   	
   	x *= screen_adjustments.xscale;
   	y *= screen_adjustments.yscale;
	
	x += screen_adjustments.x1;
	y += screen_adjustments.y1;
	
	float cury = y;
	
	float xscale = width / ((float)img->width);
	float yscale = height / ((float)img->height);
	
	float xstep = (((float)POLYWH) * xscale) * screen_adjustments.xscale;
	float ystep = (((float)POLYWH) * yscale) * screen_adjustments.yscale;
	
	for(i = 0; i < img->vert_polys; ++i)
	{
		float curx = x;
		for(k = 0; k < img->horiz_polys; ++k)
		{
			draw_poly(curx, cury, xstep, ystep, &img->polys[i*img->horiz_polys+k], color, z);
			curx += xstep;
		}
		cury += ystep;
	}
}

void rotate_point(float x, float y, float* xp, float* yp, float angle_rad)
{
	*xp = (cos(angle_rad) * x) - (sin(angle_rad) * y);
	*yp = (sin(angle_rad) * x) + (cos(angle_rad) * y);
}

void Image_DrawRotated(Image* img, float x, float y, float scale, uint32 color, float z,
					   float pivotu, float pivotv, float angle_rad)
{
	uint32 i,k;
   	
   	x *= screen_adjustments.xscale;
   	y *= screen_adjustments.yscale;

	x += screen_adjustments.x1;
	y += screen_adjustments.y1;

	float xstep = (((float)POLYWH) * scale) * screen_adjustments.xscale;
	float ystep = (((float)POLYWH) * scale) * screen_adjustments.yscale;
	
	float swidth = (((float)img->width) * scale) * screen_adjustments.xscale;
	float sheight = (((float)img->height) * scale) * screen_adjustments.yscale;
	
	float startx = -(swidth * pivotu);
	float starty = -(sheight * pivotv);
	
	rotate_point(startx, starty, &startx, &starty, angle_rad);
	
	startx += x;
	starty += y;
	
	float width_xstep = cos(angle_rad) * xstep;
	float width_ystep = sin(angle_rad) * xstep;
	
	float height_xstep = sin(angle_rad) * ystep;
	float height_ystep = cos(angle_rad) * ystep;
	
	pvr_vertex_t verts[4];

	for(i = 0; i < img->vert_polys; ++i)
	{
		float curx = startx - (height_xstep * i);
		float cury = starty + (height_ystep * i);
		for(k = 0; k < img->horiz_polys; ++k)
		{
			float xmult = 1.0f;
			float ymult = 1.0f;
			s_image_poly* pol = img->polys + (i*img->horiz_polys+k);
			if (pol->hasendu)
				xmult = (pol->endu + UVDIFF);
			if (pol->hasendv)
				ymult = (pol->endv + UVDIFF);
			
			verts[1].x = curx;
			verts[1].y = cury;
			verts[3].x = curx + (width_xstep * xmult);
			verts[3].y = cury + (width_ystep * xmult);
			verts[0].x = curx - (height_xstep * ymult);
			verts[0].y = cury + (height_ystep * ymult);
			verts[2].x = verts[0].x + (width_xstep * xmult);
			verts[2].y = verts[0].y + (width_ystep * xmult);
			
			draw_poly4(verts, pol, color, z);
			curx += width_xstep;
			cury += width_ystep;
		}
	}
}
