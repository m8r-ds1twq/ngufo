@echo off
call "%VS80COMNTOOLS%vsvars32.bat"
call version.cmd
vcbuild.exe /build vs2005\sockets\regex\regex.vcproj Release
if ERRORLEVEL 1 goto :EOF
vcbuild.exe /build vs2005\sockets\zlib\zlib.vcproj Release
if ERRORLEVEL 1 goto :EOF
vcbuild.exe /rebuild vs2005\ui\ui.vcproj Release
if ERRORLEVEL 1 goto :EOF
del /s /q dist\
mkdir dist
xcopy /y "vs2005\ui\Pocket PC 2003 (ARMV4)\Release\Bombus.exe" dist\
xcopy /s /y resources\*.* dist\
xcopy /s /y gsgetfile\dll\*.* dist\
xcopy /y copying.txt dist\
xcopy /y news.txt dist\
xcopy /y ..\Patchs\*.* dist\
echo !WARNING! > dist\!non_ofical_build!
echo http://ngufo.googlecode.com/ >> dist\!non_ofical_build! 
pkzipc -add -rec -path=relative "dist\bombus-%OF_VER%-%REVN% [%REVDATE%].zip" dist\*.*
pkzipc -add -rec -path=relative "dist\Bombus_PE_%OF_VER%-%REVN% [%REVDATE%]" dist\Bombus.exe
pkzipc -add -rec -path=relative "dist\Bombus_PE_%OF_VER%-%REVN% [%REVDATE%]" dist\!non_ofical_build!
echo F | xcopy "dist\bombus-%OF_VER%-%REVN% [%REVDATE%].zip" "dist\bombus-ng.zip"
rem ftp -s:ftp.inc