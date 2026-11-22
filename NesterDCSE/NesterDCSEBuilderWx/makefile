# created by VisualWx
# Project Name : NesterDCSEBuilderWx.pbc
DLLWRAP = dllwrap 
WINDRES = windres.exe 
CC=gcc 
CPP=g++ 
OBJDIR= 
EXELIBDLL = NesterDCSEBuilderWx.exe
EXENAME = NesterDCSEBuilderWx.exe
DLLNAME = NesterDCSEBuilderWx.dll
DEFNAME = NesterDCSEBuilderWx.def
LIBNAME = NesterDCSEBuilderWx.a
RES  = 
LIBRARIES=  -L"C:/wxWidgets-2.6.2/lib" -L"C:/wxWidgets-2.6.2/contrib/lib"
INCLUDES =  -I"C:/wxWidgets-2.6.2/include" -I"C:/wxWidgets-2.6.2/contrib/include" -I"C:/wxWidgets-2.6.2/lib/wx/include/msw-ansi-release-2.6" -I"" -I"C:/wxWidgets-2.6.2/lib/wx/include/msw-ansi-release-static-2.6"
OBJLINKS =  $(RES) $(OBJDIR)MyDlg.o $(OBJDIR)App.o $(OBJDIR)VwXApp.o $(OBJDIR)XML.o $(OBJDIR)crc32cool.o $(OBJDIR)main_stuff.o $(OBJDIR)nesdb.o $(OBJDIR)xml_serialize.o $(OBJDIR)cdbuildthread.o
OBJDEL =  $(RES) $(OBJDIR)MyDlg.o $(OBJDIR)App.o $(OBJDIR)VwXApp.o $(OBJDIR)XML.o $(OBJDIR)crc32cool.o $(OBJDIR)main_stuff.o $(OBJDIR)nesdb.o $(OBJDIR)xml_serialize.o $(OBJDIR)cdbuildthread.o
ifeq ($(REBUILD),ON)
all: clean $(EXELIBDLL)
.PHONY : all
clean: 
	-rm $(OBJDEL) $(EXELIBDLL) 
endif
CXXFLAGS = $(INCLUDES) -D__WXMSW__ -mthreads -O2 -Wall -fno-rtti -fno-pcc-struct-return -fstrict-aliasing 
LINKFLAGS = -mwindows -lwx_msw_stc-2.6 -lwx_msw_plot-2.6 -lwx_msw_gizmos-2.6 -lwx_msw_core-2.6 -lwx_base-2.6 -lwxtiff-2.6 -lwxjpeg-2.6 -lwxpng-2.6 -lwxzlib-2.6 -lwxregex-2.6 -lwxexpat-2.6 -mthreads -lrpcrt4 -loleaut32 -lole32 -luuid -lodbc32 -loleaut32 -lwinspool -lwinmm -lshell32 -lcomctl32 -lcomdlg32 -lctl3d32 -ladvapi32 -lwsock32 -lgdi32 -lwx_base_net-2.6 -lwsock32 -s
$(EXELIBDLL):$(OBJLINKS)
	$(CPP) $(OBJLINKS) -o $(EXENAME) $(LIBRARIES) $(LINKFLAGS) $(CXXFLAGS) 
 $(OBJDIR)MyDlg.o : ./MyDlg.cpp 
	$(CPP) -c  ./MyDlg.cpp  -o  $(OBJDIR)MyDlg.o  $(CXXFLAGS)
 $(OBJDIR)App.o : ./App.cpp 
	$(CPP) -c  ./App.cpp  -o  $(OBJDIR)App.o  $(CXXFLAGS)
 $(OBJDIR)VwXApp.o : ./VwXApp.cpp 
	$(CPP) -c  ./VwXApp.cpp  -o  $(OBJDIR)VwXApp.o  $(CXXFLAGS)
 $(OBJDIR)XML.o : ./XML.c 
	$(CPP) -c  ./XML.c  -o  $(OBJDIR)XML.o  $(CXXFLAGS)
 $(OBJDIR)crc32cool.o : ./crc32cool.cpp 
	$(CPP) -c  ./crc32cool.cpp  -o  $(OBJDIR)crc32cool.o  $(CXXFLAGS)
 $(OBJDIR)main_stuff.o : ./main_stuff.cpp 
	$(CPP) -c  ./main_stuff.cpp  -o  $(OBJDIR)main_stuff.o  $(CXXFLAGS)
 $(OBJDIR)nesdb.o : ./nesdb.cpp 
	$(CPP) -c  ./nesdb.cpp  -o  $(OBJDIR)nesdb.o  $(CXXFLAGS)
 $(OBJDIR)xml_serialize.o : ./xml_serialize.cpp 
	$(CPP) -c  ./xml_serialize.cpp  -o  $(OBJDIR)xml_serialize.o  $(CXXFLAGS)
 $(OBJDIR)cdbuildthread.o : ./cdbuildthread.cpp 
	$(CPP) -c  ./cdbuildthread.cpp  -o  $(OBJDIR)cdbuildthread.o  $(CXXFLAGS)

