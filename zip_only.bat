@echo off
call version.cmd
echo !WARNING! > dist\!non_ofical_build!
echo http://ngufo.googlecode.com/ >> dist\!non_ofical_build! 
pkzipc -add -rec -path=relative "dist\bombus-%OF_VER%-%REVN% [%REVDATE%].zip" dist\*.*
pkzipc -add -rec -path=relative "dist\Bombus_PE_%OF_VER%-%REVN% [%REVDATE%]" dist\Bombus.exe
pkzipc -add -rec -path=relative "dist\Bombus_PE_%OF_VER%-%REVN% [%REVDATE%]" dist\!non_ofical_build!
echo F | xcopy "dist\bombus-%OF_VER%-%REVN% [%REVDATE%].zip" "dist\bombus-ng.zip"