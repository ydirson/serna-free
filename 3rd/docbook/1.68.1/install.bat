@echo off
echo @echo off >%THIRD_DIR%\bin\docbook2html.cmd
echo set DOCBOOK_DIR=%INST_SRCDIR%\>> %THIRD_DIR%\bin\docbook2html.cmd
echo set XSLTPROC=%THIRD_DIR%\bin\xsltproc.cmd>> %THIRD_DIR%\bin\docbook2html.cmd
echo call %THIRD_DIR%\docbook\docbook2html.cmd %%*>> %THIRD_DIR%\bin\docbook2html.cmd
