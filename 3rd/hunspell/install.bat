@echo off
copy /y %INST_SRCDIR%\hunspell.dll %THIRD_DIR%\lib\
cp -rv %INST_SRCDIR%/dict %THIRD_DIR%/hunspell/
