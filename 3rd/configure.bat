@echo on

setlocal

set PATH=C:\Perl\bin;c:\cygwin\bin;%PATH%
set CYGWIN=nontsec

set VCVARS32=C:\Program Files\Microsoft Visual Studio 9.0\VC\bin\vcvars32.bat
set VCVARS32_2K3=C:\Program Files\Microsoft Visual Studio .NET 2003\VC7\bin\vcvars32.bat

if exist "%VCVARS32%" goto vs2k8
if exist "%VCVARS32_2K3%" goto vs2k3

echo Cannot find MS Visual C++
goto end

:vs2k8
call "%VCVARS32%"
goto cfg

:vs2k3
call "%VCVARS32_2K3%"

:cfg

set CONFIGURE=%0
set DTYPE=release
set TYPE=static
:parse

if "%1" == "" goto config
    if "%1" == "static" set TYPE=%1
    if "%1" == "dynamic" set TYPE=%1
    if "%1" == "debug" set DTYPE=%1
    if "%1" == "release" set DTYPE=%1
    shift
rem endif
goto parse

:config

if "%top_srcdir%" == "" set top_srcdir=%~dp0..
if not exist %top_srcdir%\tools\buildsys\tmake\bin\tmake goto no_top_srcdir
set cwd=%CD%
cd /d %top_srcdir%
set top_srcdir=%CD%
cd /d %cwd%
set buildsys=%top_srcdir%\tools\buildsys

if exist %CD%\buildopts.bat call %CD%\buildopts.bat

set TMAKEPATH=%buildsys%;%buildsys%/tmake/lib/win32-msvc
set TMAKE=perl -- %buildsys%/tmake/bin/tmake

if exist %CD%\defaults.pro set defaults=%CD%\defaults.pro
if exist %CD%\defaults.local.pro set local_defaults=%CD%\defaults.local.pro

set main_srcdir=%top_srcdir%
set top_srcdir=%CD%

set ARGS=PLATFORM=win32 "CONFIG=win32 %DTYPE% %TYPE%" top_srcdir=%top_srcdir%
set ARGS=%ARGS% %defaults% %local_defaults% main_srcdir=%main_srcdir% 
set ARGS=%ARGS% THIRD_DIR=%top_srcdir% %CD%\%PROJECT_FILE% -o %MAKEFILE% 
set ARGS=%ARGS% defaults=%defaults% local_defaults=%local_defaults% 
set ARGS=%ARGS% options=%OPTIONS% CONFIGURE=%CONFIGURE% %TMAKE_PARAMS%
set ARGS=%ARGS% "TMAKE=%TMAKE%"

%TMAKE% %ARGS%

goto end

:no_top_srcdir

echo Can't find %buildsys%\tmake\bin\tmake
exit /b 1

:end
