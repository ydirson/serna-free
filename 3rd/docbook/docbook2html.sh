#!/bin/sh

[ -z "${DOCBOOK_DIR}" -o ! -d "${DOCBOOK_DIR}" ] && echo Invalid DOCBOOK_DIR && exit 1

OUTPUTFILE="$1" && shift
[ -z "${OUTPUTFILE}" ] && OUTPUTFILE=-
[ -z "${DOCBOOK2HTML_XSL}" ] && DOCBOOK2HTML_XSL=docbook.xsl

${XSLTPROC} -o ${OUTPUTFILE} ${XSLTPROC_PARAMS} ${DOCBOOK_DIR}/html/${DOCBOOK2HTML_XSL} $@
