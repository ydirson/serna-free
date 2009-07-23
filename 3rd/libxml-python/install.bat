@echo on

set PYTHON=%THIRD_DIR%\bin\python
for /f %%P in ('%PYTHON% -c "import sys;print sys.prefix"') do set PYPREFIX=%%P

set XMLMODSDIR=%PYPREFIX%\Lib\site-packages\libxmlmods
if not exist %XMLMODSDIR% md %XMLMODSDIR%

for %%F in (libxml2mod*.pyd libxsltmod*.pyd) do xcopy /q /y %%F %XMLMODSDIR%\
for %%F in (*.py) do xcopy /q /y %%F %XMLMODSDIR%\..\
type nul > %XMLMODSDIR%\__init__.py
