
if exist %THIRD_DIR%\qtpropertybrowser\include rd /s /q %THIRD_DIR%\qtpropertybrowser\include
xcopy /e /c /h /k /y /i %INST_SRCDIR%\src %THIRD_DIR%\qtpropertybrowser\include

mt -nologo -manifest %THIRD_DIR%\lib\qtpropertybrowser.dll.manifest -outputresource:%THIRD_DIR%\lib\qtpropertybrowser.dll;2
