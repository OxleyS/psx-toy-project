@echo off
setlocal

set BUILDDIR=build
set SOURCEDIR=source
set DISTDIR=dist
set MMGMOBJDIR=C:\psyq\beta\mmgm\OBJ
set LIBDIR=C:\psyq\lib
set MKPSXISODIR=..\mkpsxiso_old

echo Clearing out previous build files...
del /Q %BUILDDIR%\*

echo Compiling...
for %%i in (%SOURCEDIR%\*.cpp) do ccpsx -O3 -c %SOURCEDIR%\%%~ni.cpp -o %BUILDDIR%\%%~ni.obj -Wall

echo Linking...
ccpsx -O3 -Xo$80010000 %BUILDDIR%\*.obj %MMGMOBJDIR%\MMGMNEW.OBJ -l %LIBDIR%\LIBPAD.LIB -l %LIBDIR%\LIBDS.LIB -l %LIBDIR%\LIBGTE.LIB -o %BUILDDIR%\main.cpe

echo Building final EXE...
cpe2x %BUILDDIR%\main.cpe

echo Building CD-ROM image...
%MKPSXISODIR%\prebuilt\mkpsxiso -y cdbuild\derp.xml

echo Build finished.

endlocal
