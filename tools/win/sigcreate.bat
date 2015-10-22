@echo off

if "%1" == "" (
    set TARGET_DIR="%~dp0"..\work
) else (
    set TARGET_DIR=%1
)

mkdir %TARGET_DIR%

copy "%~dp0"sigmake.bat       %TARGET_DIR%
copy "%~dp0"sigserver.bat     %TARGET_DIR%
copy "%~dp0"WinController.cpp %TARGET_DIR%
copy "%~dp0"WinWorld.xml      %TARGET_DIR%

@echo on

