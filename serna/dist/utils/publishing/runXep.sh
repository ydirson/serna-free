#!/bin/sh -x

XEPDIR1=/usr/local/xep
XEPDIR2=/usr/local/RenderX/XEP

XEP=${XEPDIR1}/xep

[ -x ${XEP} ] || XEP=${XEPDIR1}/xep.sh

[ -x ${XEP} ] || XEP=${XEPDIR2}/xep

[ -x ${XEP} ] && exec ${XEP} -xml "$SERNA_XML_SRCFULLPATH" \
    -xsl "$SERNA_XSL_STYLESHEET" -out "$SERNA_OUTPUT_FILE"

echo XEP script cannot be found, please make sure XEP is installed in ${XEPDIR1} or ${XEPDIR2}
exit 1
