@echo off

set SRCDIR=%1&shift
set DSTDIR=%1& shift
set RELDIR=%1& shift
set NMSFX=

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
if "%DTYPE%" == "debug" set NMSFX=d

@echo on
xcopy /y /q /r %SRCDIR%\antlr%NMSFX%.lib %DSTDIR%\lib\
xcopy /y /q /e /c /r %SRCDIR%\lib\cpp\*.* %DSTDIR%\antlr\lib\cpp\
xcopy /y /q /e /c /r %SRCDIR%\antlr\*.* %DSTDIR%\antlr\antlr\
