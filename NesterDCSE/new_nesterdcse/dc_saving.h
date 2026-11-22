#ifndef __DC_SAVING_H
#define __DC_SAVING_H

#include "types.h"
#include "nes.h"

bool8 sram_save_to_vmu (maple_device_t *dev, NES *emu);
bool8 sram_load_from_vmu (maple_device_t *dev, NES *emu);

bool8 disk_save_to_vmu (maple_device_t *dev, NES *emu);
bool8 disk_load_from_vmu (maple_device_t *dev, NES *emu);

bool8 snap_save_to_vmu (maple_device_t *dev, NES *emu, int slot);
bool8 snap_load_from_vmu (maple_device_t *dev, NES *emu, int slot);

#endif

