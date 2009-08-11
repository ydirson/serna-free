for %%K in (%CONFIG%) do if "%%K" == "debug" set DEBUG=_d

cd /d %INST_SRCDIR% && nmake install

for %%M in (QtAssistant QtCore QtGui QtNetwork QtSql QtSvg QtXml) do (
    xcopy /y /q /r %INST_SRCDIR%\%%M\%%M%DEBUG%.pyd %THIRD_DIR%\lib\
)

for %%P in (pylupdate pyrcc) do (
    del /q %THIRD_DIR%\bin\%%P4.bat
    echo PATH=+;%THIRD_DIR%\lib| perl %THIRD_DIR%\instwrapper.pl %INST_SRCDIR%\%%P %%P4 %THIRD_DIR%\bin %%P4.bat
)
rem     xcopy /y /q /r %INST_SRCDIR%\%%P\%%P4.exe %THIRD_DIR%\bin\

echo set PATH=%%PATH%%;%THIRD_DIR%\lib> %THIRD_DIR%\bin\pyuic4.bat
echo set PYTHONPATH=%%PATH%%;%THIRD_DIR%\lib>> %THIRD_DIR%\bin\pyuic4.bat
type %INST_SRCDIR%\pyuic\pyuic4.bat >> %THIRD_DIR%\bin\pyuic4.bat
type NUL > %THIRD_DIR%\lib\pyqt-init.py.stub

copy /y %THIRD_DIR%\pyqt\MANIFEST.pyqt %THIRD_DIR%\pyqt
