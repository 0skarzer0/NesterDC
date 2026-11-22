#define _NES_MAPPER_CPP_

#include "nester.h"
#include "nes.h"
#include "nes_mapper.h"

#include "debug.h"

#define MASK_BANK(bank,mask) (bank) = ((bank) & (mask))

#ifdef NESTER_DEBUG
  #define VALIDATE_ROM_BANK(bank) \
    MASK_BANK(bank,ROM_mask); \
    ASSERT((bank) < num_8k_ROM_banks) \
    if((bank) >= num_8k_ROM_banks) \
    { \
      LOG("Illegal ROM bank switch: " << (int)(bank) << "/" << (int)num_8k_ROM_banks << endl); \
      return; \
    }

  #define VALIDATE_VROM_BANK(bank) \
    MASK_BANK(bank,VROM_mask); \
    ASSERT((bank) < num_1k_VROM_banks) \
    if((bank) >= num_1k_VROM_banks) \
    { \
      LOG("Illegal VROM bank switch: " << (int)(bank) << "/" << (int)num_1k_VROM_banks << endl); \
      return; \
    }
#else
  #define VALIDATE_ROM_BANK(bank) \
    MASK_BANK(bank,ROM_mask); \
    if((bank) >= num_8k_ROM_banks) return;
  #define VALIDATE_VROM_BANK(bank) \
    MASK_BANK(bank,VROM_mask); \
    if((bank) >= num_1k_VROM_banks) return;
#endif

bool8 NES_mapper::initialize(NES* parent, NES_PPU *ppu)
{
  parent_NES = parent;
  attached_ppu = ppu;
  
  if(parent_NES->ROM->get_unifmapper_num())
  {
	  num_16k_ROM_banks = parent_NES->ROM->get_UNIF_psize_16k();
	  num_8k_ROM_banks = num_16k_ROM_banks<<1;
	  num_1k_VROM_banks = parent_NES->ROM->get_UNIF_csize_8k()<<3;
  }
  else
  {
	  num_8k_ROM_banks = 2 * parent_NES->ROM->get_num_16k_ROM_banks();
	  num_1k_VROM_banks = 8 * parent_NES->ROM->get_num_8k_VROM_banks();
  }
  
  ROM_banks  = parent_NES->ROM->get_ROM_banks();
  VROM_banks = parent_NES->ROM->get_VROM_banks();
  
  uint32 probe;
  
	ROM_mask  = 0xFFFF;
	VROM_mask = 0xFFFF;

	for(probe = 0x8000; probe; probe >>= 1)
	{
		if((num_8k_ROM_banks-1) & probe)
			break;
		ROM_mask >>= 1;
	}
	for(probe = 0x8000; probe; probe >>= 1)
	{
		if((num_1k_VROM_banks-1) & probe)
			break;
		VROM_mask >>= 1;
	}
  
  return true;
}

void NES_mapper::set_CPU_banks(uint32 bank4_num, uint32 bank5_num,
                               uint32 bank6_num, uint32 bank7_num)
{
  nes6502_context *context;

	VALIDATE_ROM_BANK(bank4_num);
	VALIDATE_ROM_BANK(bank5_num);
	VALIDATE_ROM_BANK(bank6_num);
	VALIDATE_ROM_BANK(bank7_num);
  
  context = nes6502_get_current_context ();
  context->mem_page[4] = ROM_banks + (bank4_num << 13); // * 0x2000
  context->mem_page[5] = ROM_banks + (bank5_num << 13);
  context->mem_page[6] = ROM_banks + (bank6_num << 13);
  context->mem_page[7] = ROM_banks + (bank7_num << 13);
  nes6502_setcontext();
}

// for mapper 40 /////////////////////////////////////////////////////////
void NES_mapper::set_CPU_banks(uint32 bank3_num,
                               uint32 bank4_num, uint32 bank5_num,
                               uint32 bank6_num, uint32 bank7_num)
{
  nes6502_context *context;

	VALIDATE_ROM_BANK(bank3_num);
	VALIDATE_ROM_BANK(bank4_num);
	VALIDATE_ROM_BANK(bank5_num);
	VALIDATE_ROM_BANK(bank6_num);
	VALIDATE_ROM_BANK(bank7_num);
  
  context = nes6502_get_current_context ();
  context->mem_page[3] = ROM_banks + (bank3_num << 13); // * 0x2000
  context->mem_page[4] = ROM_banks + (bank4_num << 13);
  context->mem_page[5] = ROM_banks + (bank5_num << 13);
  context->mem_page[6] = ROM_banks + (bank6_num << 13);
  context->mem_page[7] = ROM_banks + (bank7_num << 13);
  nes6502_setcontext();
}


//////////////////////////////////////////////////////////////////////////
void NES_mapper::set_PPU_banks(uint32 bank0_num, uint32 bank1_num,
                               uint32 bank2_num, uint32 bank3_num,
                               uint32 bank4_num, uint32 bank5_num,
                               uint32 bank6_num, uint32 bank7_num)
{
	VALIDATE_VROM_BANK(bank0_num);
	VALIDATE_VROM_BANK(bank1_num);
	VALIDATE_VROM_BANK(bank2_num);
	VALIDATE_VROM_BANK(bank3_num);
	VALIDATE_VROM_BANK(bank4_num);
	VALIDATE_VROM_BANK(bank5_num);
	VALIDATE_VROM_BANK(bank6_num);
	VALIDATE_VROM_BANK(bank7_num);

  attached_ppu->PPU_VRAM_banks[0] = VROM_banks + ((bank0_num & VROM_mask) << 10); // * 0x400
  attached_ppu->PPU_VRAM_banks[1] = VROM_banks + ((bank1_num & VROM_mask) << 10);
  attached_ppu->PPU_VRAM_banks[2] = VROM_banks + ((bank2_num & VROM_mask) << 10);
  attached_ppu->PPU_VRAM_banks[3] = VROM_banks + ((bank3_num & VROM_mask) << 10);
  attached_ppu->PPU_VRAM_banks[4] = VROM_banks + ((bank4_num & VROM_mask) << 10);
  attached_ppu->PPU_VRAM_banks[5] = VROM_banks + ((bank5_num & VROM_mask) << 10);
  attached_ppu->PPU_VRAM_banks[6] = VROM_banks + ((bank6_num & VROM_mask) << 10);
  attached_ppu->PPU_VRAM_banks[7] = VROM_banks + ((bank7_num & VROM_mask) << 10);
}

void NES_mapper::set_PPU_banks_MMC5(uint32 bank_num)
{
	VALIDATE_VROM_BANK(bank_num);
	attached_ppu->PPU_VRAM_banks[4] = attached_ppu->PPU_VRAM_banks[0] = VROM_banks + ((bank_num++ & VROM_mask) << 10); // * 0x400
	VALIDATE_VROM_BANK(bank_num);
	attached_ppu->PPU_VRAM_banks[5] = attached_ppu->PPU_VRAM_banks[1] = VROM_banks + ((bank_num++ & VROM_mask) << 10);
	VALIDATE_VROM_BANK(bank_num);
	attached_ppu->PPU_VRAM_banks[6] = attached_ppu->PPU_VRAM_banks[2] = VROM_banks + ((bank_num++ & VROM_mask) << 10);
	VALIDATE_VROM_BANK(bank_num);
	attached_ppu->PPU_VRAM_banks[7] = attached_ppu->PPU_VRAM_banks[3] = VROM_banks + ((bank_num++ & VROM_mask) << 10);
}

// for UNIF
void NES_mapper::set_CPU_bank_unif(uint32 kn, uint8 pagen)
{
	nes6502_context *context;
	VALIDATE_ROM_BANK(kn);
//	LOG("ROM bank switch: " << HEX(blockn,2) << ", " << HEX(kn,2) << endl);
	context = nes6502_get_current_context ();
	context->mem_page[pagen] = ROM_banks + (kn << 13); // * 0x2000
	nes6502_setcontext();
	//SCHERZOset_genie();
}

void NES_mapper::set_VRAM_bank_unif(uint32 kn, uint8 pagen)
{
	VALIDATE_VROM_BANK(kn);
	parent_NES->ppu->PPU_VRAM_banks[pagen] = VROM_banks + (kn << 10); // * 0x400
//	parent_NES->ppu->set_pattype(0, 1);
}