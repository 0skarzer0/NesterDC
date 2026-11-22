@Echo off 
 Path = c:\mingw\bin;c:\mingw\include;c:\mingw\lib;c:\mingw\;C:\Projects\NesterDCSEBuilderWx\VwxUtility;%Path%;
@Echo on 
 @mingw32-make.exe -f makefile REBUILD=OFF
