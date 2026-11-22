#include "aica_types.h"
#include "aica_utils.h"
#include "aica.h"

/****************** Timer *******************************************/

extern volatile int timer;

void
timer_wait(int jiffies) 
{
  int fin = timer + jiffies;
  while (timer <= fin)
    ;
}


/****************** Main Program ************************************/

#define ARM_OFFSET 0
static volatile uint32 *start  = (uint32*)(0x10000 + ARM_OFFSET);
static volatile uint32 *init_done = (uint32*)(0x10004 + ARM_OFFSET);
static volatile uint32 *play_start = (uint32*)(0x10008 + ARM_OFFSET);
static volatile uint32 *rate = (uint32*)(0x10010 + ARM_OFFSET);
static volatile uint32 *bits = (uint32*)(0x10014 + ARM_OFFSET);
static volatile uint32 *buf_len = (uint32*)(0x10018 + ARM_OFFSET);
static volatile uint32 *position  = (uint32*)(0x10020 + ARM_OFFSET);

static volatile int32 *pan1  = (int32*)(0x10024 + ARM_OFFSET);
static volatile int32 *pan2  = (int32*)(0x10028 + ARM_OFFSET);
static volatile int32 *pan3  = (int32*)(0x10030 + ARM_OFFSET);
static volatile int32 *pan4  = (int32*)(0x10034 + ARM_OFFSET);
static volatile int32 *pan5  = (int32*)(0x10038 + ARM_OFFSET);
static volatile int32 *pan6  = (int32*)(0x10040 + ARM_OFFSET);

#define BASE_ADDRESS 0x11000
#define BASE_ADDRESS_2 (BASE_ADDRESS+0x2000)
#define BASE_ADDRESS_3 (BASE_ADDRESS_2+0x2000)
#define BASE_ADDRESS_4 (BASE_ADDRESS_3+0x2000)
#define BASE_ADDRESS_5 (BASE_ADDRESS_4+0x2000)
#define BASE_ADDRESS_6 (BASE_ADDRESS_5+0x2000)

extern int __bss_start, end;

static inline int calc_aica_pan(int x)
{
	if (x == 0x80)
		return 0;
	else if (x < 0x80)
		return 0x10 | ((0x7f - x) >> 3);
	else
		return (x - 0x80) >> 3;
}

int arm_main() 
{
	uint8 *bss_start = (uint8 *)(&__bss_start);
	uint8 *bss_end = (uint8 *)(&end);
	int sound_bits;

	*start = 0;
	*init_done = 0;
	*play_start = 0;
	
	*pan1 = 128-25;
	*pan2 = 128+25;
	*pan3 = 128;
	*pan4 = 128;
	*pan5 = 128;
	*pan6 = 128;

	/* Clear out the BSS area */
	memset(bss_start, 0, bss_end - bss_start);

	/* Initialize the AICA part of the SPU */
	aica_init();

	/* Observe channel 0 */
	SNDREG8(0x280d) = 0;

	while (!*start) 
	{
	*init_done = 1;
	timer_wait (10);
	}

	switch (*bits)
	{
	case 8:
	sound_bits = SM_8BIT;
	break;
	case 16:
	sound_bits = SM_16BIT;
	break;
	default:
	/* ADPCM don't work now */
	return 0;
	}

	aica_play (0, BASE_ADDRESS, sound_bits, 0, *buf_len / (*bits / 8), *rate, 255, 128-25, 1);
	aica_play (1, BASE_ADDRESS_2, sound_bits, 0, *buf_len / (*bits / 8), *rate, 255, 128+25, 1);
	aica_play (2, BASE_ADDRESS_3, sound_bits, 0, *buf_len / (*bits / 8), *rate, 255, 128, 1);
	aica_play (3, BASE_ADDRESS_4, sound_bits, 0, *buf_len / (*bits / 8), *rate, 255, 128, 1);
	aica_play (4, BASE_ADDRESS_5, sound_bits, 0, *buf_len / (*bits / 8), *rate, 255, 128, 1);
	aica_play (5, BASE_ADDRESS_6, sound_bits, 0, *buf_len / (*bits / 8), *rate, 255, 128, 1);

	*play_start = 1;

	for (;;) 
	{
		CHNREG8(0, 36) = calc_aica_pan(*pan1);
		CHNREG8(1, 36) = calc_aica_pan(*pan2);
		CHNREG8(2, 36) = calc_aica_pan(*pan3);
		CHNREG8(3, 36) = calc_aica_pan(*pan4);
		CHNREG8(4, 36) = calc_aica_pan(*pan5);
		CHNREG8(5, 36) = calc_aica_pan(*pan6);

		SNDREG8(0x280d) = 0;
		*position = SNDREG32(0x2814);
		timer_wait (15);
	}

	return 0;
}

