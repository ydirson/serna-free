
if exist %THIRD_DIR%\qtsingleapplication\include rd /s /q %THIRD_DIR%\qtsingleapplication\include
xcopy /e /c /h /k /y /i %INST_SRCDIR%\src %THIRD_DIR%\qtsingleapplication\include

set MANIFEST=%THIRD_DIR%\lib\qtsingleapplication.dll.manifest
if exist %MANIFEST% mt -nologo -manifest %MANIFEST% -outputresource:%THIRD_DIR%\lib\qtsingleapplication.dll;2

echo NAME = qtsingleapplication > %THIRD_DIR%\lib\qtsingleapplication.pkg
echo LIBS = %THIRD_DIR%\lib\qtsingleapplication.lib >> %THIRD_DIR%\lib\qtsingleapplication.pkg
echo INCLUDES = %THIRD_DIR%\qtsingleapplication\include >> %THIRD_DIR%\lib\qtsingleapplication.pkg
