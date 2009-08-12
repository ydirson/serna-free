@echo off

set DSTDIR=%THIRD_DIR%
set PYEXEDIR=%INST_SRCDIR%\PCbuild

for %%C in (%CONFIG%) do (
    if "%%C" == "static" set TYPE=%%C
    if "%%C" == "dynamic" set TYPE=%%C
    if "%%C" == "debug" set DTYPE=%%C
    if "%%C" == "release" set DTYPE=%%C
    if "%%C" == "vc2003" set PYEXEDIR=%INST_SRCDIR%\PC\VS7.1
)

:inst

if "%DTYPE%" == "debug" set NMSFX=_d

echo @set PYTHONHOME=%INST_SRCDIR%> %DSTDIR%\bin\python.bat
echo @set PATH=%%PATH%%;%THIRD_DIR%\lib;%PYEXEDIR%>> %DSTDIR%\bin\python.bat
echo @%PYEXEDIR%\python%NMSFX%.exe %%*>> %DSTDIR%\bin\python.bat

rem Needed for sip
@echo on

xcopy /y /q /r %INST_SRCDIR%\PC\pyconfig.h %INST_SRCDIR%\Include\

mkdir %INST_SRCDIR%\libs
if "%DTYPE%" == "debug" (
    copy /y /b %PYEXEDIR%\python26_d.lib %INST_SRCDIR%\libs\python26.lib
    xcopy /y /q /r %PYEXEDIR%\python26_d.lib %INST_SRCDIR%\libs\
)
if "%DTYPE%" == "release" (
    xcopy /y /q /r %PYEXEDIR%\python26.lib %INST_SRCDIR%\libs\
)

rem End sip section

xcopy /y /q /e /c /r %INST_SRCDIR%\Include %DSTDIR%\python\Include\
xcopy /y /q /r %PYEXEDIR%\python26%NMSFX%.lib %DSTDIR%\lib\
xcopy /y /q /r %PYEXEDIR%\python26%NMSFX%.dll %DSTDIR%\lib\
xcopy /y /q /r %PYEXEDIR%\*%NMSFX%.pyd %DSTDIR%\python\modules\
xcopy /y /q /r %INST_SRCDIR%\PC\pyconfig.h %DSTDIR%\python\

copy %THIRD_DIR%\python\MANIFEST.python %THIRD_DIR%\python\MANIFEST
