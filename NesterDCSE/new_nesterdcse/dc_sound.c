#include "kos.h"
#include "dc/g2bus.h"
#include "types.h"
#include "dc_sound.h"
#include <core/dc_utils.h>

#define ARM_OFFSET 0xa0800000
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
#define BASE_ADDRESS_2 (0x11000+0x2000)

static const uint8 stream_driver[] = {
#include "sound/aica_fw.h"
};

void dc_sound_set_pan(uint32 ch, int32 pan)
{
	switch (ch)
	{
		case 0:
			*pan1 = pan;
			break;
		case 1:
			*pan2 = pan;
			break;
		case 2:
			*pan3 = pan;
			break;
		case 3:
			*pan4 = pan;
			break;
		case 4:
			*pan5 = pan;
			break;
		case 5:
			*pan6 = pan;
			break;
	}
}

int32 dc_sound_get_pan(uint32 ch)
{
	switch (ch)
	{
		case 0:
			return *pan1;
		case 1:
			return *pan2;
		case 2:
			return *pan3;
		case 3:
			return *pan4;
		case 4:
			return *pan5;
		case 5:
			return *pan6;
	}
	return 128;
}

void dc_sound_init (int sample_rate, int sample_bits, int buffer_len)
{
  *init_done = 0; /* need before spu_memload */
  g2_fifo_wait ();
  
  spu_disable();
  spu_memload (0, stream_driver, sizeof(stream_driver));
  spu_enable ();
  
  *rate = sample_rate;
  *bits = sample_bits;
  *buf_len = buffer_len;
  g2_fifo_wait ();
  
  for (;;)
  {
    if (*init_done) break;
    timer_spin_sleep (100);
  }
  
  *start = TRUE;
  g2_fifo_wait ();
  
  for (;;)
  {
    if (*play_start) break;
    timer_spin_sleep (100);
  }
}


void dc_sound_shutdown (void)
{
  spu_disable();
}


uint32 dc_sound_get_baseaddr (void)
{
  return BASE_ADDRESS;
}


uint32 dc_sound_get_baseaddr2 (void)
{
  return BASE_ADDRESS_2;
}


uint32 dc_sound_get_position (void)
{
  uint32 pos = *position;
  g2_fifo_wait ();
  return pos;
}



