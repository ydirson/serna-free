#! /bin/sh -x
XML_CATALOG_FILES=$SERNA_DATA_DIR/plugins/dita/catalog-11.xml export XML_CATALOG_FILES
if [[ "$OSTYPE" = darwin* ]]
then
$SERNA_DATA_DIR/MacOS/xsltproc --xinclude -o "temp_serna_map.xml" "$2/topicmerge.xsl" "$1"
$SERNA_DATA_DIR/MacOS/xsltproc --xinclude -o "temp_serna_map.fo" "$2/dita2fo-shell.xsl" "temp_serna_map.xml"
else
$SERNA_DATA_DIR/bin/xsltproc --xinclude -o "temp_serna_map.xml" "$2/topicmerge.xsl" "$1"
$SERNA_DATA_DIR/bin/xsltproc --xinclude -o "temp_serna_map.fo" "$2/dita2fo-shell.xsl" "temp_serna_map.xml"
fi
fop.sh -fo "temp_serna_map.fo" -pdf "$3"
rm temp_serna_map.xml
rm temp_serna_map.fo

