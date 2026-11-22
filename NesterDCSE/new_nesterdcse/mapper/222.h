#ifndef __NES_MAPPER_222_H
#define __NES_MAPPER_222_H

#include "nes_mapper.h"

/////////////////////////////////////////////////////////////////////
// Mapper 222
class NES_mapper222 : public NES_mapper
{
	friend class NES_SNSS;

public:
	void  Reset();
	void  MemoryWrite(uint32 addr, uint8 data);

protected:
private:
};
/////////////////////////////////////////////////////////////////////

#endif
