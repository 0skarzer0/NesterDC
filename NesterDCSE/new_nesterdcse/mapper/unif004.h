#ifndef __NES_MAPPER_UNIF_004_H
#define __NES_MAPPER_UNIF_004_H

// 
class NES_UNIFmapper4 : public NES_mapper
{
	friend class NES_SNSS;
	
public:
	void  Reset();
	void  MemoryWrite(uint32 addr, uint8 data);
	void  MemoryWriteLow(uint32 addr, uint8 data);
	void  HSync(uint32 scanline);

protected:
	uint8 regs[5], dregs[8];
	int irq_counter, irq_latch;
	uint8 irq_enabled;

	void oMMC3PRG(uint8);
	void oMMC3CHR(uint8);
	void setchr1(uint8, uint8);
	void setchr2(uint8, uint8);

private:
};
/////////////////////////////////////////////////////////////////////

#endif
