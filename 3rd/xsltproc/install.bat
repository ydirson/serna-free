
set INST_DIR=%THIRD_DIR%\xsltproc\install
set INSTNAME=xsltproc-bin.exe

for %%E in (bin\xsltproc.exe lib\libxslt.dll lib\libexslt.dll) do (
    mt -nologo -manifest %INST_SRCDIR%\win32\bin.msvc\%%~nxE.manifest -outputresource:%INST_DIR%\%%E;2
)

copy /b /y %INST_DIR%\bin\xsltproc.exe %THIRD_DIR%\bin\%INSTNAME%
copy /b /y %INST_DIR%\lib\lib*xslt.dll %THIRD_DIR%\lib\
copy /b /y %INST_DIR%\lib\lib*xslt.lib %THIRD_DIR%\lib\

del /q %THIRD_DIR%\bin\xsltproc.cmd
echo PATH+=%THIRD_DIR%\lib;| perl %THIRD_DIR%\instwrapper.pl %THIRD_DIR%\bin %INSTNAME% %THIRD_DIR%\bin xsltproc.cmd

PKGFILE=%THIRD_DIR%\lib\xsltproc.pkg
echo NAME = xsltproc > %PKGFILE%
echo LIBS = %THIRD_DIR%\lib\libxslt.lib >> %PKGFILE%
echo INCLUDES = %THIRD_DIR%\xsltproc\install\include >> %PKGFILE%
echo XSLTPROC = %THIRD_DIR%\bin\xsltproc >> %PKGFILE%
