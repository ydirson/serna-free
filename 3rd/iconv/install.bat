xcopy /y %INST_SRCDIR%\lib\iconv.dll %THIRD_DIR%\lib\
mt.exe -nologo -manifest %INST_SRCDIR%\lib\iconv.dll.manifest -outputresource:%THIRD_DIR%\lib\iconv.dll;2

xcopy /y %INST_SRCDIR%\lib\iconv.lib %THIRD_DIR%\lib\
xcopy /y %INST_SRCDIR%\include\iconv.h %THIRD_DIR%\iconv\

copy /y %THIRD_DIR%\iconv\MANIFEST.iconv %THIRD_DIR%\iconv\MANIFEST
