#ifndef __NES_MAPPER_140_H
#define __NES_MAPPER_140_H

#include "nes_mapper.h"

/////////////////////////////////////////////////////////////////////
// Mapper 140
class NES_mapper140 : public NES_mapper
{
	friend class NES_SNSS;

public:
	void  Reset();
	void  MemoryWriteSaveRAM(uint32 addr, uint8 data);

protected:
private:
};
/////////////////////////////////////////////////////////////////////

#endif
