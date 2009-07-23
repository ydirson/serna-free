@echo off

setlocal

set PATH=C:\Perl\bin;c:\cygwin\bin;C:\Program Files\Inno Setup 5;%PATH%
set CYGWIN=nontsec
call "C:\Program Files\Microsoft Visual Studio 9.0\VC\bin\vcvars32.bat"

call configure.bat %*

nmake || exit /b

cd /d serna\app && nmake pkg
