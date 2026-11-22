#include "types.h"
#include "dc_sound_mgr.h"
#include <core/dc_utils.h>
#include <math.h>

bool8 
dc_sound_mgr::initialize ()
{
	//buffer = (uint8*) memalign(32, MY_BUF_LEN);
	//buffer2 = (uint8*) memalign(32, MY_BUF_LEN);
	buffers.rect0 = (uint8*) memalign(32, MY_BUF_LEN);
	buffers.rect1 = (uint8*) memalign(32, MY_BUF_LEN);
	buffers.tri = (uint8*) memalign(32, MY_BUF_LEN);
	buffers.noise = (uint8*) memalign(32, MY_BUF_LEN);
	buffers.dmc = (uint8*) memalign(32, MY_BUF_LEN);
	buffers.ex = (uint8*) memalign(32, MY_BUF_LEN);
	
	aBufs[0] = buffers.rect0;
	aBufs[1] = buffers.rect1;
	aBufs[2] = buffers.tri;
	aBufs[3] = buffers.noise;
	aBufs[4] = buffers.dmc;
	aBufs[5] = buffers.ex;
	
	g_spuAddrs[0] = RECT0_ADDR;
	g_spuAddrs[1] = RECT1_ADDR;
	g_spuAddrs[2] = TRI_ADDR;
	g_spuAddrs[3] = NOISE_ADDR;
	g_spuAddrs[4] = DMC_ADDR;
	g_spuAddrs[5] = EX_ADDR;
	
  //memset(buffer, 0, MY_BUF_LEN);
  //memset(buffer2, 0, MY_BUF_LEN);
  memset(buffers.rect0, 0, MY_BUF_LEN);
  memset(buffers.rect1, 0, MY_BUF_LEN);
  memset(buffers.tri, 0, MY_BUF_LEN);
  memset(buffers.noise, 0, MY_BUF_LEN);
  memset(buffers.dmc, 0, MY_BUF_LEN);
  memset(buffers.ex, 0, MY_BUF_LEN);
  buffer_locked = false;
  clear_buffer ();
  dc_sound_init (SAMPLE_RATE, SAMPLE_BITS, SOUND_BUF_LEN);
  spu_dma_init();
  
  return true;
}


dc_sound_mgr::~dc_sound_mgr()
{
	dc_sound_shutdown();
	spu_dma_shutdown();
	//free(buffer);
	//free(buffer2);
	free(buffers.rect0);
	free(buffers.rect1);
	free(buffers.tri);
	free(buffers.noise);
	free(buffers.dmc);
	free(buffers.ex);
}


void dc_sound_mgr::clear_buffer()
{
  spu_memset(RECT0_ADDR, 0, SOUND_BUF_LEN);
  spu_memset(RECT1_ADDR, 0, SOUND_BUF_LEN);
  spu_memset(TRI_ADDR, 0, SOUND_BUF_LEN);
  spu_memset(NOISE_ADDR, 0, SOUND_BUF_LEN);
  spu_memset(DMC_ADDR, 0, SOUND_BUF_LEN);
  spu_memset(EX_ADDR, 0, SOUND_BUF_LEN);
}


bool8 dc_sound_mgr::lock(sound_buf_pos which, nes_buffers_t** bufs, uint32* buf_len)
{
  if (buffer_locked) return false;
  
  buffer_locked = true;
  curbuffer = (which == SOUND_BUF_HIGH);
  *bufs = &buffers;
  *buf_len = MY_BUF_LEN;
  return true;
}

static volatile uint32 curDMAbuf;

void dma_chain(ptr_t data)
{
	if (curDMAbuf >= 6)
		return;
	dc_sound_mgr* p = (dc_sound_mgr*) data;
	uint32 temp = curDMAbuf;
	curDMAbuf++;
	dcache_flush_range((uint32) p->aBufs[temp], MY_BUF_LEN);
	spu_dma_transfer(p->aBufs[temp], p->g_spuAddrs[temp] + MY_BUF_LEN * p->curbuffer, MY_BUF_LEN, 0, dma_chain, (ptr_t) p);
}

void dc_sound_mgr::unlock()
{
	if (!buffer_locked)
		return;

	buffer_locked = false;
	
	curDMAbuf = 1;

	dcache_flush_range((uint32) buffers.rect0, MY_BUF_LEN);
	spu_dma_transfer(buffers.rect0, RECT0_ADDR + MY_BUF_LEN * curbuffer, MY_BUF_LEN, 0, dma_chain, (ptr_t) this);

	//spu_memload(dc_sound_get_baseaddr () + MY_BUF_LEN * curbuffer, buffer, MY_BUF_LEN);
	//spu_memload(dc_sound_get_baseaddr2 () + MY_BUF_LEN * curbuffer, buffer2, MY_BUF_LEN);
}


/* return the alternation of SOUND_BUF_LOW and SOUND_BUF_HIGH,
   because of NES_APU::DoFrame implement. */
sound_mgr::sound_buf_pos dc_sound_mgr::get_currently_playing_half()
{
  uint32 val = dc_sound_get_position () * (SAMPLE_BITS / 8);
  
  if (val < SOUND_BUF_LEN / 2)
    return SOUND_BUF_LOW;
  else
    return SOUND_BUF_HIGH;
}

int16 dc_sound_mgr::getAvg(uint32 channelNum)
{
	float total = 0;
	int16* chBuf = (int16*) aBufs[channelNum];
	for (uint32 k = 0; k < (MY_BUF_LEN/2); ++k)
	{
		int16 temp = chBuf[k];
		if (temp < 0)
			temp = -temp;
		total += temp;
	}
	return (int16) (total / ((float)(MY_BUF_LEN/2)));
}

/*
int16 dc_sound_mgr::getAvg(uint32 channelNum)
{
	int16 max = 0;
	int16* chBuf = (int16*) aBufs[channelNum];
	for (uint32 k = 0; k < (MY_BUF_LEN/2); ++k)
	{
		int16 temp = chBuf[k];
		if (temp < 0)
			temp = -temp;
		//total += temp;
		if (temp > max)
			max = temp;
	}
	return max;
}
*/
