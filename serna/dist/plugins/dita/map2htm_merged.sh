#!/bin/sh -x
XML_CATALOG_FILES=$SERNA_DATA_DIR/plugins/dita/catalog-11.xml export XML_CATALOG_FILES
temp=temp_serna_map_$$.xml
if [[ "$OSTYPE" = darwin* ]]
then
$SERNA_DATA_DIR/MacOS/xsltproc --xinclude -o $temp "$2/topicmerge.xsl" "$1"
$SERNA_DATA_DIR/MacOS/xsltproc --xinclude -o "$3" "$2/dita2html.xsl" $temp
else
$SERNA_DATA_DIR/bin/xsltproc --xinclude -o $temp "$2/topicmerge.xsl" "$1"
$SERNA_DATA_DIR/bin/xsltproc --xinclude -o "$3" "$2/dita2html.xsl" $temp
fi
rm -f $temp
