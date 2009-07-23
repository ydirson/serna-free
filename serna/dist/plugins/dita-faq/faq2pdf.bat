
set DITA_OT_DIR=%SERNA_DITA_OT_DIR%

set ANT_TARGET=dita2pdf
set ANT_MAKEFILE=%DITA_OT_DIR%\build.xml
set ANT_HOME=%DITA_OT_DIR%\tools\ant

if not "" == "%SERNA_TEMPDIR%" goto mktempdir
set TEMPDRIVE=
for %%D in (z y x w v u t s r q p o n m l k j i h g f e d) do if not exist %%D: (
    subst %%D: "%TEMP%"&& set TEMPDRIVE=%%D&& goto tmpdfound
)
set SERNA_TEMPDIR=%TEMP%\_%SERNA_XML_SRCFILENAME%_
goto mktempdir

:tmpdfound
set SERNA_TEMPDIR=%TEMPDRIVE%:\_%SERNA_XML_SRCFILENAME%_

:mktempdir
if not exist "%SERNA_TEMPDIR%" mkdir "%SERNA_TEMPDIR%"

set CLASSPATH=%SERNA_TEMPDIR%\..;%DITA_OT_DIR%\lib;%DITA_OT_DIR%\lib;%DITA_OT_DIR%\lib\dost.jar;%DITA_OT_DIR%\lib\resolver.jar;%DITA_OT_DIR%\lib\fop.jar;%DITA_OT_DIR%\lib\avalon-framework-cvs-20020806.jar;%DITA_OT_DIR%\lib\batik.jar;%DITA_OT_DIR%\lib\xalan.jar;%DITA_OT_DIR%\lib\xercesImpl.jar;%DITA_OT_DIR%\lib\xml-apis.jar;%DITA_OT_DIR%\lib\icu4j.jar;%CLASSPATH%
set PATH=%PATH%;%ANT_HOME%\bin

set CATMGRPROPS=%SERNA_TEMPDIR%\..\CatalogManager.properties
set XML_CATALOG_FILES=%XML_CATALOG_FILES: file:=;file:%
echo catalogs=%XML_CATALOG_FILES%> %CATMGRPROPS%
echo relative-catalogs=false>> %CATMGRPROPS%
echo prefer=public>> %CATMGRPROPS%
echo static-catalog=yes>> %CATMGRPROPS%
rem echo verbosity=256>> %CATMGRPROPS%

rem FIXME: java looks for dita-ot-settings.ent included from catalog-11.xml here
set DITA_OT_SETTINGS=%SERNA_OUTPUT_DIR%\dita-ot-settings.ent
copy "%DITA_OT_DIR%\..\dita-ot-settings.ent" "%DITA_OT_SETTINGS%"

set ANT_ARGS=%ANT_ARGS% -Dargs.target=init -Dargs.transtype=fo -Dargs.input="%SERNA_XML_SRCFULLPATH:\=/%"
set ANT_ARGS=%ANT_ARGS% -Doutput.dir="%SERNA_OUTPUT_DIR:\=/%" -Ddita.temp.dir="%SERNA_TEMPDIR:\=/%"
set ANT_ARGS=%ANT_ARGS% -Dargs.logdir="%SERNA_TEMPDIR:\=/%"

set ANT_ARGS=%ANT_ARGS% -Dargs.xsl="%SERNA_XSL_STYLESHEET:\=/%"

call "%ANT_HOME%\bin\ant.bat" %ANT_ARGS% -f "%ANT_MAKEFILE%" %ANT_TARGET%

if exist "%CATMGRPROPS%" del /q "%CATMGRPROPS%"
if exist "%DITA_OT_SETTINGS%" del /q "%DITA_OT_SETTINGS%"

if not "" == "%TEMPDRIVE%" subst %TEMPDRIVE%: /d
