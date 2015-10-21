@echo off

if "%1" == "" (
    set TARGET_DIR=%~dp0..\work
) else (
    set TARGET_DIR=%1
)

mkdir %TARGET_DIR%

copy %~dp0sigmake.bat         %TARGET_DIR%
copy %~dp0sigserver.bat       %TARGET_DIR%
copy %~dp0ControllerTempl.cpp %TARGET_DIR%

@echo on

