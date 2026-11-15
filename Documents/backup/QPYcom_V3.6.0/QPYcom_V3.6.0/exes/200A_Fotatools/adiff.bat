
@echo off

del /s /q /f dfota_file.bin
adiff.exe -f Falcon_EVB_QSPI_Nor_LWG_Only_Nontrusted_PM802_LPDDR2.blf -o old -n new -x dfota_file.bin
