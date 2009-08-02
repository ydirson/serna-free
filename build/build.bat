@echo off

call configure.bat || exit /b 1

nmake || exit /b 1

cd /d serna\app && nmake pkg
