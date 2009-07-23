#! /bin/sh -x

# This will work you have a "saxon" in your path that
# takes the following arguments: 
# saxon.sh sourceFile.xml stylesheetFile.xsl outputFileName

exec saxon.sh "$SERNA_XML_SRCFULLPATH" "$SERNA_XSL_STYLESHEET" \
    "$SERNA_OUTPUT_FILE"
