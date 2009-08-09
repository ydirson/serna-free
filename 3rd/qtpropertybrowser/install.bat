
if exist %THIRD_DIR%\qtpropertybrowser\include rd /s /q %THIRD_DIR%\qtpropertybrowser\include
xcopy /e /c /h /k /y /i %INST_SRCDIR%\src %THIRD_DIR%\qtpropertybrowser\include

set MANIFEST=%THIRD_DIR%\lib\qtpropertybrowser.dll.manifest
if exist %MANIFEST% mt -nologo -manifest %MANIFEST% -outputresource:%THIRD_DIR%\lib\qtpropertybrowser.dll;2

echo NAME = qtpropertybrowser > %THIRD_DIR%\lib\qtpropertybrowser.pkg
echo LIBS = %THIRD_DIR%\lib\qtpropertybrowser.lib >> %THIRD_DIR%\lib\qtpropertybrowser.pkg
echo INCLUDES = %THIRD_DIR%\qtpropertybrowser\include >> %THIRD_DIR%\lib\qtpropertybrowser.pkg

echo include:MANIFEST.qtpropertybrowser > %THIRD_DIR%\qtpropertybrowser\MANIFEST
