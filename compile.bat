@echo off
setlocal

set BUILDDIR=build
set SOURCEDIR=source
set DISTDIR=dist
set MMGMOBJDIR=C:\psyq\beta\mmgm\OBJ
set LIBDIR=C:\psyq\lib

echo Clearing out previous build files...
del /Q %BUILDDIR%\*

echo Compiling...
for %%i in (%SOURCEDIR%\*.c) do ccpsx -O3 -c %SOURCEDIR%\%%~ni.c -o %BUILDDIR%\%%~ni.obj -Wall

echo Linking...
ccpsx -O3 -Xo$80010000 %BUILDDIR%\*.obj %MMGMOBJDIR%\MMGMNEW.OBJ -l %LIBDIR%\LIBPAD.LIB -o %BUILDDIR%\main.cpe

echo Building final EXE...
cpe2x %BUILDDIR%\main.cpe
move %BUILDDIR%\MAIN.EXE %DISTDIR%\MAIN.EXE

endlocal