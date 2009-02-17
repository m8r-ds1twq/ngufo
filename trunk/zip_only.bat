@echo off
echo !WARNING! > dist\!non_ofical_build!
pkzipc -add -rec -path=relative "dist\bombus-%REVN% [%REVDATE%] UFO.zip" dist\*.*
pkzipc -add -rec -path=relative "dist\Bombus_PE_only" dist\Bombus.exe
pkzipc -add -rec -path=relative "dist\Bombus_PE_only" dist\!non_ofical_build!
echo F | xcopy "dist\bombus-%REVN% [%REVDATE%] UFO.zip" "dist\bombus-ng.zip"