@rem This will work if you have a directory with xsltproc.exe and 
@rem xep.bat in your path

xsltproc --xinclude -o temp_serna_map.xml %2/topicmerge.xsl %1
xsltproc --xinclude -o temp_serna_map.fo %2/dita2fo-shell.xsl temp_serna_map.xml
xep.bat -fo temp_serna_map.fo -out %3
del temp_serna_map.xml
del temp_serna_map.fo
