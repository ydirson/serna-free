#!/bin/sh -x

rm -f ${THIRD_DIR}/qtpropertybrowser/include
ln -s ${INST_SRCDIR}/src ${THIRD_DIR}/qtpropertybrowser/include

cat <<EOF > ${THIRD_DIR}/lib/qtpropertybrowser.pkg
NAME     = qtpropertybrowser
INCLUDES = ${THIRD_DIR}/qtpropertybrowser/include
LFLAGS   = -L${THIRD_DIR}/lib
LIBS     = qtpropertybrowser
EOF
