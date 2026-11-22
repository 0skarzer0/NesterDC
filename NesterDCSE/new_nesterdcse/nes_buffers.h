#ifndef _NES_BUFFERS_H_
#define _NES_BUFFERS_H_

#define BASE_ADDRESS 0x11000
#define BASE_ADDRESS_2 (BASE_ADDRESS+0x2000)
#define BASE_ADDRESS_3 (BASE_ADDRESS_2+0x2000)
#define BASE_ADDRESS_4 (BASE_ADDRESS_3+0x2000)
#define BASE_ADDRESS_5 (BASE_ADDRESS_4+0x2000)
#define BASE_ADDRESS_6 (BASE_ADDRESS_5+0x2000)

#define RECT0_ADDR	BASE_ADDRESS
#define RECT1_ADDR	BASE_ADDRESS_2
#define TRI_ADDR	BASE_ADDRESS_3
#define NOISE_ADDR	BASE_ADDRESS_4
#define DMC_ADDR	BASE_ADDRESS_5
#define EX_ADDR		BASE_ADDRESS_6

typedef struct nes_buffers_s
{
	uint8* rect0;
	uint8* rect1;
	uint8* tri;
	uint8* noise;
	uint8* dmc;
	uint8* ex;
} nes_buffers_t;

#endif
