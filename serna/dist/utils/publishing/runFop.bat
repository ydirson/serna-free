rem This will work you have a "fop.bat" in your path that
rem takes the following arguments: 
rem fop.bat -xml sourceFile.xml -xsl stylesheetFile.xsl -pdf outputFileName

fop.bat -xml "%SERNA_XML_SRCFULLPATH%" -xsl "%SERNA_XSL_STYLESHEET%" -pdf "%SERNA_OUTPUT_FILE%"
