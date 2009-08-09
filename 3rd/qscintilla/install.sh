#!/bin/sh -x

. ${THIRD_DIR}/functions

install_shlib ../${INST_RELDIR}/Qt4 ${THIRD_DIR}/lib libqscintilla2

test -e ${THIRD_DIR}/qscintilla/translations || mkdir -p ${THIRD_DIR}/qscintilla/translations
cp -f ../../${INST_RELDIR}/Qt4/*.qm ${THIRD_DIR}/qscintilla/translations/

cat <<EOF > ${THIRD_DIR}/lib/qscintilla.pkg
NAME     = qscintilla
INCLUDES = ${THIRD_DIR}/qscintilla
LFLAGS   = -L${THIRD_DIR}/lib
LIBS     = qscintilla2
EOF
