#!/bin/sh

FOP=`which fop 2>/dev/null` || FOP=`which fop.sh 2>/dev/null`

[ ! -x "${FOP}" -a `uname -s` = "Darwin" ] && FOP=/usr/local/fop-0.93/fop

if [ -z "${FOP}" ]; then
    echo "Make sure 'fop' or 'fop.sh' is in your PATH"
else
    set -x
    exec ${FOP} -xml "$SERNA_XML_SRCFULLPATH" -xsl "$SERNA_XSL_STYLESHEET" \
        -pdf "$SERNA_OUTPUT_FILE"
fi
