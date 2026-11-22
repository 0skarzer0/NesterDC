#ifndef _DC_MOUSE_H_
#define _DC_MOUSE_H_

#include <kos.h>
#include "types.h"

struct DCMouse
{
	maple_device_t* dev;
	mouse_state_t state;
	mouse_state_t old_state;
};

extern inline void DCMouse_Init(struct DCMouse* p)
{
	p->dev = NULL;
	memset(&p->old_state, 0, sizeof(mouse_state_t));
	memset(&p->state, 0, sizeof(mouse_state_t));
}

extern inline void DCMouse_Poll(struct DCMouse* p)
{
	p->old_state = p->state;
	p->state = *((mouse_state_t*) maple_dev_status (p->dev));
}

extern inline bool8 DCMouse_IsPressed(struct DCMouse* p, uint16 which)
{
	return ((p->state.buttons & which) ? 1 : 0);
}

extern inline bool8 DCMouse_JustPressed(struct DCMouse* p, uint16 which)
{
	return ((p->state.buttons & which) && !(p->old_state.buttons & which));
}

extern struct DCMouse g_mice[4];

#endif

