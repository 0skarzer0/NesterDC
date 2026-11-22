#ifndef _DC_MOUSE_H_
#define _DC_MOUSE_H_

#include <kos.h>

struct DCMouse
{
	maple_device_t* dev;
	mouse_state_t state;
	mouse_state_t old_state;
};

#define DCMouse_Init(p)\
{\
	(p)->dev = NULL;\
	memset(&(p)->old_state, 0, sizeof(mouse_state_t));\
	memset(&(p)->state, 0, sizeof(mouse_state_t));\
}

#define DCMouse_Poll(p)\
{\
	(p)->old_state = (p)->state;\
	(p)->state = *((mouse_state_t*) maple_dev_status ((p)->dev));\
}

#define DCMouse_IsPressed(p, which) (((p)->state.buttons & which) == (which))

#define DCMouse_JustPressed(p, which) ((((p)->state.buttons & which) == (which)) && !(((p)->old_state.buttons & which) != (which)))

extern struct DCMouse g_mice[4];

#endif

