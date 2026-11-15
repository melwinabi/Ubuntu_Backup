@echo off

set CURDIR=%~dp0

echo= 
FotaToolkit.exe -d config/ec718p.json BINPKG old_to_new_kernel.par %CURDIR%\old\at_command.hbinpkg %CURDIR%\new\at_command.hbinpkg

if exist "%CURDIR%\old\ap_application.bin" (
echo need diff application
FotaToolkit.exe -d config/ec718p.json APP old_to_new_app %CURDIR%\old\ap_application.bin %CURDIR%\new\ap_application.bin

FotaToolkit.exe -m config/ec718p.json  old_to_new %CURDIR%\old_to_new_kernel.par %CURDIR%\old_to_new_app.par
)