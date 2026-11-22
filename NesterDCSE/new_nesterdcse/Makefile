# NesterDC SE Makefile
# by Scherzo

TARGET = nesterdcse.elf

ORIG_MAPPER_OBJS =  mapper/000.o mapper/001.o mapper/002.o mapper/003.o mapper/004.o
ORIG_MAPPER_OBJS += mapper/005.o mapper/006.o mapper/007.o mapper/008.o mapper/009.o
ORIG_MAPPER_OBJS += mapper/010.o mapper/011.o mapper/013.o mapper/015.o mapper/016.o
ORIG_MAPPER_OBJS += mapper/017.o mapper/018.o mapper/019.o mapper/020.o mapper/021.o
ORIG_MAPPER_OBJS += mapper/022.o mapper/023.o mapper/024.o mapper/025.o mapper/026.o
ORIG_MAPPER_OBJS += mapper/032.o mapper/033.o mapper/034.o mapper/040.o mapper/041.o
ORIG_MAPPER_OBJS += mapper/042.o mapper/043.o mapper/044.o mapper/045.o mapper/046.o
ORIG_MAPPER_OBJS += mapper/047.o mapper/048.o mapper/049.o mapper/050.o mapper/051.o
ORIG_MAPPER_OBJS += mapper/052.o mapper/057.o mapper/058.o mapper/060.o mapper/062.o
ORIG_MAPPER_OBJS += mapper/064.o mapper/065.o mapper/066.o mapper/067.o mapper/068.o
ORIG_MAPPER_OBJS += mapper/069.o mapper/070.o mapper/071.o mapper/072.o mapper/073.o
ORIG_MAPPER_OBJS += mapper/074.o mapper/075.o mapper/076.o mapper/077.o mapper/078.o
ORIG_MAPPER_OBJS += mapper/079.o mapper/080.o mapper/082.o mapper/083.o mapper/085.o
ORIG_MAPPER_OBJS += mapper/086.o mapper/087.o mapper/088.o mapper/089.o mapper/090.o
ORIG_MAPPER_OBJS += mapper/091.o mapper/092.o mapper/093.o mapper/094.o mapper/095.o
ORIG_MAPPER_OBJS += mapper/096.o mapper/097.o mapper/099.o mapper/100.o mapper/101.o
ORIG_MAPPER_OBJS += mapper/105.o mapper/112.o mapper/113.o mapper/114.o mapper/115.o
ORIG_MAPPER_OBJS += mapper/117.o mapper/118.o mapper/119.o mapper/122.o mapper/140.o
ORIG_MAPPER_OBJS += mapper/151.o mapper/160.o mapper/180.o mapper/181.o mapper/182.o
ORIG_MAPPER_OBJS += mapper/183.o mapper/185.o mapper/187.o mapper/188.o mapper/189.o
ORIG_MAPPER_OBJS += mapper/222.o mapper/225.o mapper/226.o mapper/227.o mapper/228.o
ORIG_MAPPER_OBJS += mapper/229.o mapper/230.o mapper/231.o mapper/232.o mapper/233.o
ORIG_MAPPER_OBJS += mapper/234.o mapper/235.o mapper/236.o mapper/237.o mapper/240.o
ORIG_MAPPER_OBJS += mapper/241.o mapper/242.o mapper/243.o mapper/244.o mapper/245.o
ORIG_MAPPER_OBJS += mapper/246.o mapper/248.o mapper/249.o mapper/250.o mapper/251.o
ORIG_MAPPER_OBJS += mapper/252.o mapper/254.o mapper/255.o mapper/nsf.o
ORIG_MAPPER_OBJS += mapper/unif001.o mapper/unif002.o mapper/unif003.o mapper/unif004.o

OBJS += $(ORIG_MAPPER_OBJS)

OBJS += dc_gg.o dc_saving.o dc_sound.o dc_sound_mgr.o getmapper.o libsnss.o nes6502.o nes.o nes_6502.o 
OBJS += nes_apu.o nes_apu_wrapper.o nes_device_cclimber.o nes_device_gun.o nes_device_hypershot.o nes_device_kbd.o 
OBJS += nes_device_mahjong.o nes_device_mic.o nes_device_paddle.o nes_mapper.o nes_pad.o nes_ppu.o nes_rom.o
OBJS += nnnesterj_cheat.o snss.o main.o nesdb.o nesdb_object.o
OBJS += explode.o unreduce.o unshrink.o unzip.o crc32cool.o

# JS Exposure
OBJS += nester_object.o

MY_AFLAGS = -Wa,-little

KOS_CFLAGS += -D__DREAMCAST__ -I. -I../libdreamscript
KOS_CFLAGS += -frename-registers -funswitch-loops -g -O3

all: rm-elf $(TARGET)

include $(KOS_BASE)/Makefile.rules

clean:
	$(KOS_MAKE) -C sound clean
	-rm -f $(TARGET) $(BIN) $(OBJS) bin/$(TARGET) bin/raw.bin bin/1ST_READ.BIN cd/1ST_READ.BIN

cleanasm:
	-rm -f $(ASM_CPU_OBJS)

rm-elf:
	-rm -f $(TARGET) $(BIN)

sound-driver:
	$(KOS_MAKE) -C sound

$(TARGET): sound-driver $(OBJS)
	$(KOS_CC) $(KOS_LDFLAGS) $(KOS_START) $(OBJS) -o $(TARGET) -g -lz -lstdc++ -ldreamscript -lm -lkosutils -limageload -lparallax -ljpeg -lkmg -lpng -lbz2 $(KOS_LIBS)
	cp $(TARGET) prestrip_$(TARGET)
	$(KOS_STRIP) $(TARGET)
	$(KOS_OBJCOPY) -R .stack -O binary $(TARGET) bin/raw.bin
	scramble bin/raw.bin bin/1ST_READ.BIN
	cp bin/1ST_READ.BIN cd/1ST_READ.BIN
	cp $(TARGET) bin/$(TARGET)

nes.o: nes_emulate_frame.cpp nes_set_controller.cpp nes_set_cycles.cpp
nes_ppu.o: render_bg.cpp render_spr.cpp do_scanline.cpp render_tile.cpp
dc_sound.o: sound/aica_fw.h

cdimg:
	mkisofs -J -R -l -joliet-long -o cd.iso cd


