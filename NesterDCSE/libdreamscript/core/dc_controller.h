#ifndef _DC_CONTROLLER_H_
#define _DC_CONTROLLER_H_

#include <kos.h>

struct DCController
{
	maple_device_t* dev;
	cont_state_t state;
	cont_state_t old_state;
};

#define DCController_Init(p)\
	(p)->dev = NULL;\
	memset(&((p)->old_state), 0, sizeof(cont_state_t));\
	memset(&((p)->state), 0, sizeof(cont_state_t));

#define DCController_Poll(p)\
	(p)->old_state = (p)->state;\
	(p)->state = *((cont_state_t*) maple_dev_status ((p)->dev));

#define DCController_IsPressed(p, which) (((p)->state.buttons & (which)) == (which))

#define DCController_JustPressed(p, which) (((p)->state.buttons & (which)) == (which) && ((p)->old_state.buttons & (which)) != (which))

extern struct DCController g_controllers[4];

#endif

