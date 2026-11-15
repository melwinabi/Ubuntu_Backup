
@echo off

del /s /q /f fota_file.bin
MakeFOTAImageRTOS.exe -f Falcon_EVB_QSPI_Nor_LWG_Only_Nontrusted_PM802_LPDDR2.blf -o fota_file.bin
