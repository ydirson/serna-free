@rem This will work if you have a directory with xsltproc.exe and in your path

xsltproc --xinclude --param "FILEREF" "''" -o %3 %2 %1
