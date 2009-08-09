copy /b %INST_SRCDIR%\aspell-*.dll %THIRD_DIR%\lib
for %%L in (%INST_SRCDIR%\aspell-*.dll) do (
    if exist %%L.manifest mt -nologo -manifest %%L.manifest -outputresource:%THIRD_DIR%\lib\%%~nxL;2
)
xcopy /y %THIRD_DIR%\aspell\lib\aspell\i386\*.rws %THIRD_DIR%\aspell\lib\aspell\
copy /y %THIRD_DIR%\aspell\MANIFEST.aspell %THIRD_DIR%\aspell\MANIFEST
