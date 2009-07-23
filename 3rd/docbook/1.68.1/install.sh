#!/bin/sh

DOCBOOK2HTML=${THIRD_DIR}/bin/docbook2html

PLATFORM=`uname -s | tr 'A-Z' 'a-z' | tr -d '\n'`

echo "#!/bin/sh"> ${DOCBOOK2HTML}
echo "DOCBOOK_DIR=\"${INST_SRCDIR}/\"; export DOCBOOK_DIR">> ${DOCBOOK2HTML}
echo "XSLTPROC=\"${THIRD_DIR}/bin/xsltproc\"; export XSLTPROC">> ${DOCBOOK2HTML}
echo "exec ${THIRD_DIR}/docbook/docbook2html.sh \$@">> ${DOCBOOK2HTML}

chmod 755 ${DOCBOOK2HTML}
