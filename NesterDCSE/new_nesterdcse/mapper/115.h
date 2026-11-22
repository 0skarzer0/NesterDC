#ifndef __NES_MAPPER_115_H
#define __NES_MAPPER_115_H

#include "nes_mapper.h"

/////////////////////////////////////////////////////////////////////
// Mapper 115
class NES_mapper115 : public NES_mapper
{
	friend class NES_SNSS;

public:
	void  Reset();
	//  void  MemoryWriteLow(uint32 addr, uint8 data);
	//  void  MemoryReadSaveRAM(uint32 addr);
	//  void  MemoryWriteSaveRAM(uint32 addr, uint8 data);
	void  MemoryWrite(uint32 addr, uint8 data);
	void  HSync(uint32 scanline);

protected:
	uint8 regs[1];

	uint32 irq_counter, irq_latch;
	uint8 irq_enabled;

private:
};
/////////////////////////////////////////////////////////////////////

#endif
