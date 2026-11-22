/*
** nester - NES emulator
** Copyright (C) 2000  Darren Ranalli
**
** This program is free software; you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation; either version 2 of the License, or
** (at your option) any later version.
**
** This program is distributed in the hope that it will be useful, 
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU 
** Library General Public License for more details.  To obtain a 
** copy of the GNU Library General Public License, write to the Free 
** Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
**
** Any permitted reproduction of these routines, in whole or in part,
** must bear this legend.
*/

#ifndef NES_PAD_H_
#define NES_PAD_H_

#include "types.h"

class NES_pad
{
public:
  NES_pad()
  {
	  nes_A = FALSE;
	  nes_B = FALSE;
	  nes_SELECT = FALSE;
	  nes_START = FALSE;
	  nes_UP = FALSE;
	  nes_DOWN = FALSE;
	  nes_LEFT = FALSE;
	  nes_RIGHT = FALSE;
  }

  bool8 nes_A;
  bool8 nes_B;
  bool8 nes_SELECT;
  bool8 nes_START;
  bool8 nes_UP;
  bool8 nes_DOWN;
  bool8 nes_LEFT;
  bool8 nes_RIGHT;
  
  uint8 get_state();
};

#endif
