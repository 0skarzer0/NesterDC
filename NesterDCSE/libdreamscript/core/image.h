#ifndef JPEG_TO_IMG
#define JPEG_TO_IMG

#include <kos.h>
#include "dc_utils.h"
#include <imageload/imageload.h>

START_EXTERN_C

typedef struct
{
	pvr_ptr_t ptr;
	pvr_poly_hdr_t hdr;
	float endu;
	float endv;
	int8 hasendu;
	int8 hasendv;
} s_image_poly;

typedef struct
{
	uint32 horiz_polys;
	uint32 vert_polys;
	s_image_poly polys[256];
	uint32 width;
	uint32 height;
	pvr_list_t list;
} Image;

void Image_Init(Image* img);

int Image_Load(Image* img, const char* filename, pvr_list_t list, int filtering, IMG_ALPHA alpha);

void Image_Destroy(Image* img);

void Image_Draw(Image* img, float x, float y, float scale, uint32 color, float z);
void Image_DrawTrue(Image* img, float x, float y, float scale, uint32 color, float z);
void Image_DrawWH(Image* img, float x, float y, float width, float height, uint32 color, float z);

void Image_DrawRotated(Image* img, float x, float y, float scale, uint32 color, float z,
					   float pivotu, float pivotv, float angle);

END_EXTERN_C

#endif

