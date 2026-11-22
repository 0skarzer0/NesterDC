#ifndef _DC_SOUND_MGR_H_
#define _DC_SOUND_MGR_H_

#include "types.h"
#include "sound_mgr.h"
#include "dc_sound.h"
#include "nes_buffers.h"

/* SOUND_BUF_LEN = 2 * (rate * buffer_length_in_frames / 60.0) * (bit / 8) */
/* (format "%x" ((lambda (x) (* 2 (/ (* 44100 x) 60) (/ 16 8))) 3)) */
/* NOTE: AICA require 32-bit (or higher) alignment. */
#define SAMPLE_RATE 44100
#define SAMPLE_BITS 16
#define SOUND_BUF_LEN 0x2000
#define MY_BUF_LEN 	  (0x2000/2)

class dc_sound_mgr : public sound_mgr
{
public:
  ~dc_sound_mgr ();
  bool8 initialize ();
  
  void reset() {}
  
  void freeze() {}
  void thaw() {}
  
  void clear_buffer();
  bool8 lock(sound_buf_pos which, nes_buffers_t** bufs, uint32* buf_len);
  void unlock();
  
  int get_sample_rate() { return SAMPLE_RATE; }
  int get_sample_bits() { return SAMPLE_BITS; }
  
  sound_buf_pos get_currently_playing_half();
  
  bool8 IsNull() { return FALSE; }
  
  uint8* getBuffer() { return buffers.rect0; }
  
  int16 getAvg(uint32 channelNum);
  
  friend void dma_chain(ptr_t data);
  friend class NesterImp;
  
private:
  //uint8* buffer;
  //uint8* buffer2;
  nes_buffers_t buffers;
  uint8* aBufs[6];
  uint32 g_spuAddrs[6];
  bool8 buffer_locked;
  int curbuffer;
};

#endif


