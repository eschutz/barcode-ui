@echo off
git clone "https://github.com/eschutz/libbarcode.git"
cd libbarcode
call winbuild.bat
cd ..
xcopy .\libbarcode\lib\libbarcode.dll .\lib /Y
xcopy .\libbarcode\include .\include  /E /Y
