@echo off
SETLOCAL EnableDelayedExpansion

SET CC=cl

SET SDIR=src
SET UIDIR=ui
SET ODIR=build
SET SRC_FILES=ui win util backend resources

SET INCLUDES_STR=/wd4068 /Iinclude /Iinclude\win

SET CFLAGS=%INCLUDES_STR%

SET LIBPATH=lib
SET BARCODELIB=libbarcode.dll
SET LIBPATH_WIN=lib\win

SET MAIN_EXE=barcode.exe

SET DEP_LIBS=
for /F %%l in ('dir /b /a-d lib\win') do (
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
link /LIBPATH:%LIBPATH_WIN% /LIBPATH:%LIBPATH% /OUT:%MAIN_EXE% %OBJS%
