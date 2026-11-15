@echo off

set CURDIR=%~dp0

echo= 
FotaToolkit.exe -d config/ec618.json BINPKG old_to_new_kernel.par %CURDIR%\old\at_command.binpkg %CURDIR%\new\at_command.binpkg

if exist "%CURDIR%\old\ap_application.bin" (
echo need diff application
FotaToolkit.exe -d config/ec618.json APP old_to_new_app %CURDIR%\old\ap_application.bin %CURDIR%\new\ap_application.bin

FotaToolkit.exe -m config/ec618.json  old_to_new %CURDIR%\old_to_new_kernel.par %CURDIR%\old_to_new_app.par
)