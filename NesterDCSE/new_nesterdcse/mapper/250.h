#ifndef __NES_MAPPER_250_H
#define __NES_MAPPER_250_H

/////////////////////////////////////////////////////////////////////
// Mapper 250
class NES_mapper250 : public NES_mapper
{
	friend class NES_SNSS;
	
public:
	void  Reset();
	//  uint8 MemoryReadLow(uint32);
	//  void  MemoryWriteLow(uint32 addr, uint8 data);
	void  MemoryWrite(uint32 addr, uint8 data);

protected:
	//  uint8 regs[1];

private:
};
/////////////////////////////////////////////////////////////////////

#endif
