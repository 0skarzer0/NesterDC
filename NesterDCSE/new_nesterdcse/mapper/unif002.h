#ifndef __NES_MAPPER_UNIF_002_H
#define __NES_MAPPER_UNIF_002_H

// 
class NES_UNIFmapper2 : public NES_mapper
{
	friend class NES_SNSS;
	
public:
	void  Reset();
	void  MemoryWrite(uint32 addr, uint8 data);
	void  MemoryWriteLow(uint32 addr, uint8 data);
	uint8  MemoryReadLow(uint32 addr);

protected:
	uint8 wram[0x1000];

private:
};
/////////////////////////////////////////////////////////////////////

#endif
