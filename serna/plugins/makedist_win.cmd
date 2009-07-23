@rem 
@echo on

if "%ISS_SCRIPT%" == "" echo "ISS_SCRIPT is not set" & exit 1
if "%DISTDIR%" == "" echo "DISTDIR is not set" & exit 1
if "%PLUGIN_NAME%" == "" echo "PLUGIN_NAME is not set" & exit 1
if "%APPVER%" == "" echo "APPVER is not set" & exit 1
if "%RELEASE%" == "" echo "RELEASE is not set" & exit 1

iscc %ISS_SCRIPT% || exit 1
if not exist Output\setup.exe echo "Output setup file doesn't exist" & exit 1
if not exist %DISTDIR% mkdir %DISTDIR%
copy /b Output\setup.exe %DISTDIR%\%PLUGIN_NAME%-%APPVER%-%RELEASE%.exe
