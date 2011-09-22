@echo off
copy /y %INST_SRCDIR%\hunspell.dll %THIRD_DIR%\lib\
xcopy /y /q /r %INST_SRCDIR%\dict %THIRD_DIR%\hunspell\
