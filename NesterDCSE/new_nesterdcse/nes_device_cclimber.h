#ifndef _NES_DEVICE_CCLIMBER_H_
#define _NES_DEVICE_CCLIMBER_H_

class NES_device_cclimber
{
public:
  bool8 initialize();
  
  uint8 ReadReg4016();	
  void WriteReg4016(NES *emu);
  
  bool8 left_UP;
  bool8 left_DOWN;
  bool8 left_LEFT;
  bool8 left_RIGHT;
  
  bool8 right_UP;
  bool8 right_DOWN;
  bool8 right_LEFT;
  bool8 right_RIGHT;
};

#endif
