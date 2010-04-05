@echo on

setlocal enableextensions
setlocal enabledelayedexpansion

set BIN_TARGETS=uic uic3 moc rcc lupdate lrelease qt3to4 assistant linguist qhelpgenerator qcollectiongenerator

for %%F in (%BIN_TARGETS%) do (
  set DSTNAME=bin\%%F.exe
  del /q %THIRD_DIR%\!DSTNAME!
  set DSTNAME=!DSTNAME:.exe=_bin.exe!
  copy /y /b %INST_SRCDIR%\bin\%%F.exe %THIRD_DIR%\!DSTNAME!
  echo @set PATH=%%PATH%%;%INST_SRCDIR%\bin;%INST_SRCDIR%\lib> %THIRD_DIR%\bin\%%~nF.bat
  echo %THIRD_DIR%\!DSTNAME! %%*>> %THIRD_DIR%\bin\%%~nF.bat
)

set DLL_TARGETS=Qt3Support QtCore QtDesignerComponents QtDesigner QtNetwork QtCLucene
set DLL_TARGETS=%DLL_TARGETS% QtGui QtSql QtSvg QtTest QtXml QtHelp QtAssistantClient
set LIB_TARGETS=qtmain

set LIBSFX=

for %%D in (%CONFIG%) do if "%%D" == "debug" set LIBSFX=d

for %%F in (%DLL_TARGETS%) do (
    xcopy /y /q %INST_SRCDIR%\lib\%%F%LIBSFX%4.lib %THIRD_DIR%\lib\
    xcopy /y /q %INST_SRCDIR%\lib\%%F%LIBSFX%4.dll %THIRD_DIR%\lib\
    echo. TARGET_DIR = %THIRD_DIR%\lib > %THIRD_DIR%\lib\%%F.pro
    echo. TARGET_FILE = %%F%LIBSFX%4.lib >> %THIRD_DIR%\lib\%%F.pro
    echo. DESTDIR = "$(THIRD_DIR)\lib" >> %THIRD_DIR%\lib\%%F.pro
    echo. TARGET_BASE = %%F%LIBSFX% >> %THIRD_DIR%\lib\%%F.pro
)

for %%F in (%LIB_TARGETS%) do (
    xcopy /y /q %INST_SRCDIR%\lib\%%F%LIBSFX%.lib %THIRD_DIR%\lib\
)

xcopy /y /q /e /c /r %INST_SRCDIR%\translations\*.* %THIRD_DIR%\qt\translations\

rd /s /q %THIRD_DIR%\qt\include
xcopy /y /q /e /c /r %INST_SRCDIR%\include\*.* %THIRD_DIR%\qt\include\
copy /b /y %INST_SRCDIR%\tools\porting\src\q3porting.xml %THIRD_DIR%\qt\q3porting.xml

dir /b /s %THIRD_DIR%\qt\include\*.h | %THIRD_DIR%\bin\python %~dp0fixincludes.py ..\..\%INST_RELDIR:qt\=%

xcopy /y /q /c /r %INST_SRCDIR%\plugins\imageformats\*.dll %THIRD_DIR%\qt\plugins\imageformats\
copy /y %THIRD_DIR%\qt\MANIFEST.qt %THIRD_DIR%\qt\MANIFEST

cd %THIRD_DIR%\qt\translations
sh -c '../../bin/lrelease.bat qt*.ts'
