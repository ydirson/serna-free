@rem This will work if you have a directory with xsltproc.exe in your path

xsltproc --xinclude -o "%SERNA_OUTPUT_FILE%" "%SERNA_XSL_STYLESHEET%" "%SERNA_XML_SRCFULLPATH%"
