@echo on

call %~dp0..\win32env.bat || goto noenv

set CONFIGURE=%0
set DTYPE=release
set TYPE=static
:parse

if "%1" == "" goto config
    if "%1" == "static" set TYPE=%1
    if "%1" == "dynamic" set TYPE=%1
    if "%1" == "debug" set DTYPE=%1
    if "%1" == "release" set DTYPE=%1
    if "%1" == "syspkg" set SYSPKG=syspkg
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

set ARGS=PLATFORM=win32 "CONFIG=win32 %DTYPE% %TYPE% %SYSPKG%" top_srcdir=%top_srcdir%
set ARGS=%ARGS% %defaults% %local_defaults% main_srcdir=%main_srcdir% 
set ARGS=%ARGS% THIRD_DIR=%top_srcdir% %CD%\%PROJECT_FILE% -o %MAKEFILE% 
set ARGS=%ARGS% defaults=%defaults% local_defaults=%local_defaults% 
set ARGS=%ARGS% options=%OPTIONS% CONFIGURE=%CONFIGURE% %TMAKE_PARAMS%
set ARGS=%ARGS% "TMAKE=%TMAKE%"

%TMAKE% %ARGS%

goto end

:noenv
echo Can't set Win32 environment variables
exit /b 1

:no_top_srcdir

echo Can't find %buildsys%\tmake\bin\tmake
exit /b 1

:end
