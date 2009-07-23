@echo off

if "%DOCBOOK_DIR%"=="" echo Invalid DOCBOOK_DIR && exit 1
if not exist "%DOCBOOK_DIR%" echo Invalid DOCBOOK_DIR && exit 1
if "%DOCBOOK2HTML_XSL%"=="" set DOCBOOK2HTML_XSL=docbook.xsl

set OUTPUTFILE=%1& shift

if "%OUTPUTFILE%"=="" set OUTPUTFILE=-

%XSLTPROC% -o %OUTPUTFILE% %XSLTPROC_PARAMS% %DOCBOOK_DIR%\html\%DOCBOOK2HTML_XSL% %1 %2 %3 %4 %5 %6 %7 %8 %9
