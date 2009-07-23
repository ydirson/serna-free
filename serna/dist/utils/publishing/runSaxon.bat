rem This will work you have a "saxon" in your path that
rem takes the following arguments: 
rem saxon.bat sourceFile.xml stylesheetFile.xsl outputFileName

saxon.bat "%SERNA_XML_SRCFULLPATH%" "%SERNA_XSL_STYLESHEET%" "%SERNA_OUTPUT_FILE%"