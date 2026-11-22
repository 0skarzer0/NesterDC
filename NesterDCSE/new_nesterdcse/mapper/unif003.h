#ifndef __NES_MAPPER_UNIF_003_H
#define __NES_MAPPER_UNIF_003_H

// 
class NES_UNIFmapper3 : public NES_mapper
{
	friend class NES_SNSS;
	
public:
	void  Reset();
	void  MemoryWrite(uint32 addr, uint8 data);
//	void  MemoryWriteLow(uint32 addr, uint8 data);
	void  MemoryWriteSaveRAM(uint32 addr, uint8 data);
protected:
	uint8 regs[1];

	void Setprg();
private:
};
/////////////////////////////////////////////////////////////////////

#endif
