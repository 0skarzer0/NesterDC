#include "fft.h"
#include "visualization.h"
#include <kos.h>
#include <math.h>

void calc_freq(int16* dest, int16* src) // 256, 512
{
	static fft_state* state = NULL;
	float tmp_out[257];
	int i;
	
	if(!state)
		state = fft_init();

	fft_perform(src,tmp_out,state);
	
	for(i = 0; i < 256; i++)
		dest[i] = ((int)sqrt(tmp_out[i + 1])) >> 8;
}
