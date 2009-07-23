@echo on

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
goto build

:vs2k3
call "%VCVARS32_2K3%"

:build

cd /d %~dp0 || goto end

(cd 3rd && call configure.bat && nmake install) || goto end

cd /d %~dp0

(cd build && call build.bat)

:end
exit /b
