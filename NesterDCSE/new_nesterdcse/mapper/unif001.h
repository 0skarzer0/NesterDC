#ifndef __NES_MAPPER_UNIF_001_H
#define __NES_MAPPER_UNIF_001_H

// 
class NES_UNIFmapper1 : public NES_mapper
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
