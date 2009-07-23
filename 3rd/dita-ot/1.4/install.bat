@echo off
echo @echo off >%THIRD_DIR%\bin\dita2html.cmd
echo set DITA_DIR=%INST_SRCDIR%\>> %THIRD_DIR%\bin\dita2html.cmd
echo if "%%JAVA_HOME%%"=="" set JAVA_HOME=%JAVA_HOME%>> %THIRD_DIR%\bin\dita2html.cmd
echo call %THIRD_DIR%\dita-ot\dita2html.cmd %%*>> %THIRD_DIR%\bin\dita2html.cmd
