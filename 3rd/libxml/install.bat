set INST_DIR=%THIRD_DIR%\libxml\install

xcopy /y %INST_DIR%\lib\libxml2.dll %THIRD_DIR%\lib\
mt -nologo -manifest %INST_SRCDIR%\win32\bin.msvc\libxml2.dll.manifest -outputresource:%THIRD_DIR%\lib\libxml2.dll;2
xcopy /y %INST_DIR%\lib\libxml2.lib %THIRD_DIR%\lib\
xcopy /e /c /h /k /y %INST_DIR%\include %THIRD_DIR%\libxml\include\

PKGFILE=%THIRD_DIR%\lib\libxml2.pkg
echo NAME = libxml2 > %PKGFILE%
echo LIBS = %THIRD_DIR%\lib\libxml2.lib >> %PKGFILE%
echo INCLUDES = %THIRD_DIR%\libxml\include >> %PKGFILE%

copy /y %top_srcdir%\MANIFEST.xml2 %top_srcidr%\MANIFEST
