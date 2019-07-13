@echo off
rem Builds barcode-ui and creates a zip archive for redistribution

SET TARGET=x86
SET VER=1.0.0
SET OUTPUT_NAME=barcode-%TARGET%-%VER%

call winbuild.bat

echo start /MIN "bin\%TARGET%\barcode.exe" > %OUTPUT_NAME%.bat
echo exit >> %OUTPUT_NAME%.bat

call PowerShell -NoLogo -NonInteractive -Command "Compress-Archive -Path %OUTPUT_NAME%.exe, LICENSE, LICENSE-THIRD-PARTY, bin, share, etc, include -CompressionLevel Optimal -Update -DestinationPath release\%OUTPUT_NAME%.zip"
