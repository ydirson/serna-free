#!/bin/sh

rm -f ${THIRD_DIR}/bin/sip ${THIRD_DIR}/lib/sip.so ${THIRD_DIR}/sip/sip.h ${THIRD_DIR}/sip/sipconfig.py
make -C ${INST_SRCDIR} install || exit $?

ln -sf ${THIRD_DIR}/sip/sip.so ${THIRD_DIR}/lib/
ln -sf ../${INST_RELDIR}/sipgen/sip ${THIRD_DIR}/bin/
ln -sf ../${INST_RELDIR}/siplib/sip.h ${THIRD_DIR}/sip/
ln -sf ${INST_SRCDIR}/sipconfig.py ${THIRD_DIR}/sip/

cat <<EOF > ${THIRD_DIR}/lib/sip.pkg
NAME     = sip
SIP      = ${THIRD_DIR}/bin/sip
INCLUDES = ${THIRD_DIR}/sip
EOF
