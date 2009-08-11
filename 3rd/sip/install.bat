
xcopy /y /q /r %INST_SRCDIR%\sipgen\sip.exe %THIRD_DIR%\bin\
xcopy /y /q /r %INST_SRCDIR%\siplib\sip.pyd %THIRD_DIR%\lib\
xcopy /y /q /r %INST_SRCDIR%\siplib\sip.lib %THIRD_DIR%\lib\
xcopy /y /q /r %INST_SRCDIR%\siplib\sip_d.pyd %THIRD_DIR%\lib\
xcopy /y /q /r %INST_SRCDIR%\siplib\sip_d.lib %THIRD_DIR%\lib\
xcopy /y /q /r %INST_SRCDIR%\siplib\sip.h %THIRD_DIR%\sip\
xcopy /y /q /r %INST_SRCDIR%\sipconfig.py %THIRD_DIR%\sip\

set PKGFILE=%THIRD_DIR%\lib\sip.pkg
echo NAME = sip > %PKGFILE%
echo INCLUDES = %THIRD_DIR%\sip >> %PKGFILE%
echo SIP_DIR = %THIRD_DIR%\sip >> %PKGFILE%
echo SIP = %THIRD_DIR%\bin\sip >> %PKGFILE%
