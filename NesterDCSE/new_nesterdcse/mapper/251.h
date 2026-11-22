#ifndef __NES_MAPPER_251_H
#define __NES_MAPPER_251_H

/////////////////////////////////////////////////////////////////////
// Mapper 251
class NES_mapper251 : public NES_mapper
{
	friend class NES_SNSS;
	
public:
	void  Reset();
	//  uint8 MemoryReadLow(uint32);
	void  MemoryWriteSaveRAM(uint32 addr, uint8 data);
	void  MemoryWrite(uint32 addr, uint8 data);

protected:
	uint8 regs[11];
	uint8 bregs[4];
	void banksync();

private:
};
/////////////////////////////////////////////////////////////////////

#endif
