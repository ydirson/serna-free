@echo off

setlocal

set PATH=C:\Perl\bin;c:\cygwin\bin;C:\Program Files\Inno Setup 5;%PATH%
set CYGWIN=nontsec

set VCVARS32=C:\Program Files\Microsoft Visual Studio 9.0\VC\bin\vcvars32.bat
set VCVARS32_2K3=C:\Program Files\Microsoft Visual Studio .NET 2003\VC7\bin\vcvars32.bat

if exist "%VCVARS32_2K3%" goto vs2k3
if exist "%VCVARS32%" goto vs2k8

:vs2k8
call "%VCVARS32%"
goto cfg

:vs2k3
call "%VCVARS32_2K3%"

:cfg

call configure.bat %*

nmake || exit /b

cd /d serna\app && nmake pkg
