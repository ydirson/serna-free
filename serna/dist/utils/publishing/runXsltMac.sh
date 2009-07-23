#!/bin/sh -x

exec "$SERNA_DATA_DIR/MacOS/xsltproc" --xinclude -o "$SERNA_OUTPUT_FILE" \
    "$SERNA_XSL_STYLESHEET" "$SERNA_XML_SRCFULLPATH"
