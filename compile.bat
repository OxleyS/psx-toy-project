@echo off
setlocal

rem Be sure psyq\bin\psyq.ini, and cdbuild\derp.yml has the correct paths set!
set PSYQ_BASE=H:\develop\psyq

rem Stuff that used to be in system-wide envvars
set COMPILER_PATH=%PSYQ_BASE%\bin
set C_INCLUDE_PATH=%PSYQ_BASE%\include
set C_PLUS_INCLUDE_PATH=%PSYQ_BASE%\include
set LIBRARY_PATH=%PSYQ_BASE%\lib
set PSX_PATH=%PSYQ_BASE%\bin
set PSYQ_PATH=%PSYQ_BASE%\bin
set PATH=%PATH%;%PSYQ_BASE%\bin
set GO32="DPMISTACK 1000000"
set G032TMP=C:\TEMP

set BUILDDIR=build
set SOURCEDIR=source
set DISTDIR=dist
set MMGMOBJDIR=%PSYQ_BASE%\beta\mmgm\OBJ
set LIBDIR=%PSYQ_BASE%\lib
set MKPSXISODIR=tools

echo Clearing out previous build files...
del /Q %BUILDDIR%\*

echo Compiling...
for %%i in (%SOURCEDIR%\*.cpp) do ccpsx -O3 -c %SOURCEDIR%\%%~ni.cpp -o %BUILDDIR%\%%~ni.obj -Wall

echo Linking...
ccpsx -O3 -Xo$80010000 %BUILDDIR%\*.obj %MMGMOBJDIR%\MMGMNEW.OBJ -l %LIBDIR%\LIBPAD.LIB -l %LIBDIR%\LIBDS.LIB -l %LIBDIR%\LIBGTE.LIB -o %BUILDDIR%\main.cpe

echo Building final EXE...
cpe2x %BUILDDIR%\main.cpe

echo Building CD-ROM image...
%MKPSXISODIR%\mkpsxiso -y cdbuild\derp.xml

echo Build finished.

endlocal
