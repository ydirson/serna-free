@rem This will work if you have a directory with xsltproc.exe and in your path

xsltproc --xinclude -o temp_serna_map.xml %2/topicmerge.xsl %1
xsltproc --xinclude -o %3 %2/dita2html.xsl temp_serna_map.xml
del temp_serna_map.xml
