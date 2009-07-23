#!/bin/sh

DITA2HTML=${THIRD_DIR}/bin/dita2html

echo "#!/bin/sh"> ${DITA2HTML}
echo "DITA_DIR=\"${INST_SRCDIR}/\"; export DITA_DIR">> ${DITA2HTML}
echo "JAVA_HOME=\"${JAVA_HOME}\"; export JAVA_HOME">> ${DITA2HTML}
echo "exec ${THIRD_DIR}/dita-ot/dita2html.sh \$@">> ${DITA2HTML}

chmod 755 ${DITA2HTML}

chmod 755 ${INST_SRCDIR}/tools/ant/bin/ant

