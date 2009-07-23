@echo on

if "%ISS_SCRIPT%" == "" echo "ISS_SCRIPT is not set" & exit 1
if "%DISTDIR%" == "" echo "DISTDIR is not set" & exit 1
if "%ISS_VER%" == "" echo "ISS_VER is not set" & exit 1
if "%RELEASE%" == "" echo "RELEASE is not set" & exit 1

for %%F in (rebase.exe) do set REBASE=%%~dp$PATH:F%%F
if not exist "%REBASE%" goto pkg
if "%REBASE%" == "c:\cygwin\bin\rebase.exe" goto pkg

cmd.exe /c dir /s /b dist\*.pyd dist\*.dll | sort > dlls.lst || exit 1
"%REBASE%" -l rebase.log 60000000 @dlls.lst

:pkg

call iscc %ISS_SCRIPT% || exit 1
if not exist Output\setup.exe echo "Output setup file doesn't exist" & exit 1
if not exist %DISTDIR% md %DISTDIR%
set BASENAME=serna-%VERSION%-%RELEASE%.exe
copy /b Output\setup.exe %DISTDIR%\%BASENAME%
@set CWD=%CD%
@cd /d %DISTDIR%
@set SERNA_DISTDIR=%CD%
@cd /d %CWD%
echo SERNA_DIST=%SERNA_DISTDIR%\%BASENAME% > serna_dist.pro
