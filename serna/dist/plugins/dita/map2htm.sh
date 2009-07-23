#!/bin/sh -x
XML_CATALOG_FILES=$SERNA_DATA_DIR/plugins/dita/catalog-11.xml export XML_CATALOG_FILES
if [[ "$OSTYPE" = darwin* ]]
then
$SERNA_DATA_DIR/MacOS/xsltproc --xinclude --param "FILEREF" "''" -o "$3" "$2" "$1"
else
$SERNA_DATA_DIR/bin/xsltproc --xinclude --param "FILEREF" "''" -o "$3" "$2" "$1"
fi


