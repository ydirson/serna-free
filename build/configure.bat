@echo off

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

set top_builddir=%CD%
if not x%top_srcdir% == x goto cfg2

set cwd=%CD%
cd ..
set top_srcdir=%CD%
cd %cwd%

if x%top_srcdir% == x%cwd% goto no_top_srcdir

if "%top_srcdir%" == "" goto no_top_srcdir
echo %top_srcdir%
:cfg2

set defaults=
set local_defaults=
set PROJECT_FILE=Makefile.pro
set MAKEFILE=Makefile

if exist %top_srcdir%\buildopts.bat call %top_srcdir%\buildopts.bat
if exist %top_srcdir%\defaults.pro set defaults=%top_srcdir%\defaults.pro
if exist %top_srcdir%\defaults.local.pro set local_defaults=%top_srcdir%\defaults.local.pro

set TMAKEPATH=%top_srcdir%\tools\buildsys;%top_srcdir%\tools\buildsys\tmake\lib\win32-msvc
set TMAKE=perl -- %top_srcdir%\tools\buildsys\tmake\bin\tmake
set TMAKE_PARAMS=PROJECT_FILE=%PROJECT_FILE% MAKEFILE=%MAKEFILE%
%TMAKE% top_builddir=%top_builddir% top_srcdir=%top_srcdir% "CONFIG+=%TYPE% %DTYPE% %KTYPE%" %defaults% %local_defaults% %top_srcdir%\%PROJECT_FILE% -o %MAKEFILE% defaults=%defaults% local_defaults=%local_defaults% options=%OPTIONS% CONFIGURE=%CONFIGURE% "TMAKE=%TMAKE%" %TMAKE_PARAMS%"

goto end

:no_top_srcdir
echo Please specify top source directory by setting parameter top_srcdir, for example
echo.
echo configure.bat top_srcdir=fullpathname
echo.
echo or, alternatively, set top_srcdir environment variable to point to the absolute pathname of top source directory

:end
