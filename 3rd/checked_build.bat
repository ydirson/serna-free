@echo off

setlocal
setlocal enabledelayedexpansion
call %~dp0..\win32env.bat

if not "%1"=="" goto make

for /f "delims=" %%D in (.subdirs) do @echo %%D && set DIRS=!DIRS! %%D

%0 %DIRS%

:make

set ROOT=%CD%

set DIR=%1& shift
if "%DIR%" == "" goto success

echo Building %DIR%...

cd %DIR%
nmake -nologo install > cbuild.log 2>&1
if not %ERRORLEVEL% == 0 goto fail
cd %ROOT%
goto make

:success
echo Build succeeded.
goto done

:fail
echo Build failed, see %DIR%\cbuild.log for details

:done
cd %ROOT%
