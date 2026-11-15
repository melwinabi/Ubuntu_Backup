@echo off

set CURDIR=%~dp0

echo= 
FotaToolkit.exe -d config/ec718s.json BINPKG old_to_new.par %CURDIR%\old\at_command.hbinpkg %CURDIR%\new\at_command.hbinpkg