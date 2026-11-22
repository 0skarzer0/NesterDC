# NesterDC SE Builder Utility
# by Scherzo

TARGET = nesterdcsebuild.exe

CFLAGS = -I/usr/local/include

OBJS += main.o nesdb.o crc32cool.o zip.o ioapi.o unzip.o XML.o

$(TARGET): $(OBJS)
	gcc $(OBJS) -o $(TARGET) -L/usr/local/lib -lz -lghttp
	strip $(TARGET)

clean:
	rm $(TARGET) $(OBJS)
