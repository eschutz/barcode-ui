@echo off
SETLOCAL EnableDelayedExpansion

SET CC=cl
rem X86 for 32-bit binaries, X64 for 64-bit binaries
SET TARGET=X86

SET SDIR=src
SET UIDIR=ui
SET ODIR=build
SET EXE_DIR=bin\%TARGET%
SET SRC_FILES=ui win util backend resources main

SET INCLUDES_STR=/wd4068 /Iinclude /Iinclude\win

SET CFLAGS=%INCLUDES_STR%

SET BARCODE_LIBPATH=lib
SET BARCODELIB=libbarcode.lib
SET LIBPATH_WIN=lib\win\%TARGET%

SET MAIN_EXE=%EXE_DIR%\barcode.exe

SET DEP_LIBS=%BARCODELIB%
for /F %%l in ('dir /b /a-d %LIBPATH_WIN%\*.lib') do (
    SET DEP_LIBS=!DEP_LIBS! %%l
)

call vsdevcmd

rem Build UI resources
glib-compile-resources %SDIR%/%UIDIR%/barcode.gresource.xml --target=%SDIR%/resources.c --sourcedir=%SDIR%/%UIDIR% --generate-source

SET OBJS=

rem Build object files
for %%f in (%SRC_FILES%) do (
    %CC% %CFLAGS% /c %SDIR%\%%f.c /Fo:%ODIR%\%%f.obj
    SET OBJS=!OBJS! %ODIR%\%%f.obj
)

rem Link
link /debug /MACHINE:%TARGET% /LIBPATH:%LIBPATH_WIN% /LIBPATH:%BARCODE_LIBPATH% /OUT:%MAIN_EXE% %OBJS% %DEP_LIBS%
