@echo off

set SRCDIR=%1& shift
set DSTDIR=%1& shift
set RELDIR=%1& shift

:parse

if "%1" == "" goto inst
    if "%1" == "static" set TYPE=%1
    if "%1" == "dynamic" set TYPE=%1
    if "%1" == "debug" set DTYPE=%1
    if "%1" == "release" set DTYPE=%1
    shift
rem endif

goto parse

:inst

@echo on

if "%TYPE%" == "dynamic" set NMSFX=103

xcopy /y /q /r %SRCDIR%\lib\sp%NMSFX%.lib %DSTDIR%\lib\
if "%TYPE%" == "dynamic" (
    mt -nologo -manifest %SRCDIR%\lib\sp%NMSFX%.dll.manifest -outputresource:%SRCDIR%\lib\sp%NMSFX%.dll;2
    xcopy /y /q /r %SRCDIR%\lib\sp%NMSFX%.dll %DSTDIR%\lib\
)
