#!/bin/sh -xe

PACKAGE=serna
PKG_DIR=${PACKAGE}-${VERSION_ID}-${RELEASE}
TGZ_NAME=${PACKAGE}-${VERSION}-${RELEASE}

LD_LIBRARY_PATH=${THIRD_DIR}/lib:/usr/local/lib:${LD_LIBRARY_PATH}
export LD_LIBRARY_PATH

inst() {
    cat $1 | sed -e "s/@VERSION@/${VERSION}/g" \
                 -e "s/@VERSION_ID@/${VERSION_ID}/g" \
                 -e "s/@RELEASE@/${RELEASE}/g" \
           > $2
}

BASE_DIR=${inst_prefix}
SERNA_DIR=${PACKAGE}-${VERSION_ID}

TR=tr
test -x /usr/xpg4/bin/tr && TR=/usr/xpg4/bin/tr

PLATFORM=`uname -s | $TR 'A-Z' 'a-z'`
if test -f ${srcdir}/${PLATFORM}/serna-postin.sh; then
    INSTSCRIPTDIR=${srcdir}/${PLATFORM}
else
    INSTSCRIPTDIR=${srcdir}/rpm
fi

case ${PLATFORM} in
    linux)
        TGZ_NAME="${TGZ_NAME}-linux"
        ;;
    sunos)
        sol_version="`uname -r | sed -e 's/.*\.//'`-`uname -p`"
        TGZ_NAME="${TGZ_NAME}-sol${sol_version}"
        ;;
    *)
        ;;
esac

inst ${INSTSCRIPTDIR}/serna-postin.sh ${BASE_DIR}/${SERNA_DIR}/bin/serna-postin.sh
inst ${INSTSCRIPTDIR}/serna-postun.sh ${BASE_DIR}/${SERNA_DIR}/bin/serna-postun.sh
chmod 755 ${BASE_DIR}/${SERNA_DIR}/bin/serna-post?n.sh

(cd ${BASE_DIR} && tar -cf - ${SERNA_DIR} | gzip -9c > ${SERNA_DIR}.tgz)

rm -f ${BASE_DIR}/${SERNA_DIR}/bin/serna-post?n.sh

test -d ${SERNA_DIR} || mkdir ${SERNA_DIR}
mv ${BASE_DIR}/${SERNA_DIR}.tgz ${SERNA_DIR}
inst ${srcdir}/tgz-install.sh ${SERNA_DIR}/install.sh
inst ${srcdir}/tgz-uninstall.sh ${SERNA_DIR}/uninstall.sh
chmod u+x ${SERNA_DIR}/install.sh ${SERNA_DIR}/uninstall.sh
inst ${srcdir}/TGZ-INSTALL ${SERNA_DIR}/INSTALL

tar -cf - \
        ${SERNA_DIR}/${SERNA_DIR}.tgz \
        ${SERNA_DIR}/install.sh \
        ${SERNA_DIR}/uninstall.sh \
        ${SERNA_DIR}/INSTALL \
    | gzip -1c > ../${TGZ_NAME}.tgz 
    
rm -rf ${SERNA_DIR}
