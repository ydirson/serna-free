#!/bin/sh -x

rm -f ${THIRD_DIR}/qtsingleapplication/include
ln -s ${INST_SRCDIR}/src ${THIRD_DIR}/qtsingleapplication/include

cat <<EOF > ${THIRD_DIR}/lib/qtsingleapplication.pkg
NAME     = qtsingleapplication
INCLUDES = ${THIRD_DIR}/qtsingleapplication/include
LFLAGS   = -L${THIRD_DIR}/lib
LIBS     = qtsingleapplication
EOF
