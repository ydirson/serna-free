@echo on

if "%DITA_DIR%"=="" echo Invalid DITA_DIR && exit 1
if not exist "%DITA_DIR%" echo Invalid DITA_DIR && exit 1

REM Set environment variables
set ANT_OPTS=-Xmx512m %ANT_OPTS%
set ANT_HOME=%DITA_DIR%tools\ant
set PATH=%DITA_DIR%tools\ant\bin;%PATH%
set CLASSPATH=%DITA_DIR%\..\..;%DITA_DIR%lib;%DITA_DIR%lib\dost.jar;%DITA_DIR%lib\resolver.jar;%DITA_DIR%lib\fop.jar;%DITA_DIR%lib\avalon-framework-cvs-20020806.jar;%DITA_DIR%lib\batik.jar;%DITA_DIR%lib\xalan.jar;%DITA_DIR%lib\xercesImpl.jar;%DITA_DIR%lib\xml-apis.jar;%DITA_DIR%lib\icu4j.jar;%CLASSPATH%
set PROPFILE=%CD%\dita2html.properties

if "%JAVA_HOME%"=="" goto check_args
if not exist "%JAVA_HOME%" goto check_args
set CLASSPATH=%CLASSPATH%;%JAVA_HOME%\lib\tools.jar

:check_args

set INPUTFILE=%1
set INPUTBASE=%~n1
if "%INPUTFILE%"=="" echo Input file not specified && exit 1

set OUTDIR=%2
echo xslt.parser=XALAN>%PROPFILE%

if "%OUTDIR%"=="" goto runant
if not exist %OUTDIR% mkdir %OUTDIR%
echo output.dir=%OUTDIR:\=\\%>>%PROPFILE%

:runant
set LOGGER=-logger org.dita.dost.log.DITAOTBuildLogger
if not "%DITA_OT_LOGDIR%"=="" echo args.logdir=%DITA_OT_LOGDIR:\=\\%>>%PROPFILE%

set DITA_TEMPDIR=%3
if not "%DITA_TEMPDIR%"=="" echo dita.temp.dir=%DITA_TEMPDIR:\=\\%>>%PROPFILE%
set INPUT=-Dargs.input=%INPUTFILE%

if not "%DITA_HTML_XSL%" == "" echo args.xsl=%DITA_HTML_XSL:\=\\%>>%PROPFILE%
echo dita.extname=.dita>>%PROPFILE%
echo transtype=xhtml>>%PROPFILE%
echo args.xhtml.toc=%INPUTBASE%>>%PROPFILE%

set ANT_OPTS=-Ddita.dir=%DITA_DIR% %INPUT%

if not "%4"=="" cd /d "%4"
ant -f %DITA_DIR%build.xml %LOGGER% -propertyfile %PROPFILE% %ANT_OPTS%
