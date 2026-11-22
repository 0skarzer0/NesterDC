#ifndef __NES_MAPPER_062_H
#define __NES_MAPPER_062_H

#include "nes_mapper.h"

/////////////////////////////////////////////////////////////////////
// Mapper 62
class NES_mapper62 : public NES_mapper
{
	friend class NES_SNSS;

public:
	void  Reset();
	void  MemoryWrite(uint32 addr, uint8 data);
	void  MemoryWriteLow(uint32 addr, uint8 data);
	void  MemoryWriteSaveRAM(uint32 addr, uint8 data);

protected:
private:
};
/////////////////////////////////////////////////////////////////////

#endif
