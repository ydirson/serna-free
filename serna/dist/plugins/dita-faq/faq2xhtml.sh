#!/bin/sh

DITA_DIR="$SERNA_DITA_OT_DIR"
ANT_TARGET=dita2xhtml
ANT_HOME="${DITA_DIR}/tools/ant"; export ANT_HOME

if [ `uname -s` = Linux -a ! -x "${JAVA_HOME}/bin/java" ]; then
    if [ -x /usr/java/default/bin/java ]; then
        JAVA_HOME=/usr/java/default
        while [ -L ${JAVA_HOME} -a ! -z "${JAVA_HOME}" ]; do
            JAVA_HOME=`readlink -qn ${JAVA_HOME}`
        done
    else
        JAVA_BIN=`ls -tR /usr/java/j*/bin/java | head -n1`
        test -x "${JAVA_BIN}" && JAVA_HOME=`dirname "${JAVA_BIN}"`
    fi
    if [ ! -d "${JAVA_HOME}" ]; then
        echo Cannot determine JAVA_HOME
        exit 1
    fi
    export JAVA_HOME
    PATH="${JAVA_HOME}/bin:${PATH}"
fi

PATH="${ANT_HOME}/bin:${PATH}"; export PATH

test -z "${SERNA_TEMPDIR}" && SERNA_TEMPDIR="/tmp/_${SERNA_XML_SRCFILENAME}"
test -d "${SERNA_TEMPDIR}" || mkdir -p "${SERNA_TEMPDIR}"
chmod 700 "${SERNA_TEMPDIR}"

JCLASSPATH=${DITA_DIR}/lib:${DITA_DIR}/lib/dost.jar:${DITA_DIR}/lib/resolver.jar
JCLASSPATH=${JCLASSPATH}:${DITA_DIR}/lib/fop.jar
JCLASSPATH=${JCLASSPATH}:${DITA_DIR}/lib/avalon-framework-cvs-20020806.jar
JCLASSPATH=${JCLASSPATH}:${DITA_DIR}/lib/batik.jar:${DITA_DIR}/lib/xalan.jar
JCLASSPATH=${JCLASSPATH}:${DITA_DIR}/lib/xercesImpl.jar:${DITA_DIR}/lib/xml-apis.jar
JCLASSPATH=${JCLASSPATH}:${DITA_DIR}/lib/icu4j.jar:${JAVA_HOME}/lib/tools.jar:${CLASSPATH}

CLASSPATH="${SERNA_TEMPDIR}/..:${JCLASSPATH}"

export CLASSPATH

CATMGRPROPS="${SERNA_TEMPDIR}/../CatalogManager.properties"
XML_CATALOG_FILES="${XML_CATALOG_FILES/\ file:/;file:}"
echo "catalogs=${XML_CATALOG_FILES}"> ${CATMGRPROPS}
echo relative-catalogs=false>> ${CATMGRPROPS}
echo prefer=public>> ${CATMGRPROPS}
echo static-catalog=yes>> ${CATMGRPROPS}
echo verbosity=1>> ${CATMGRPROPS}

if [ ! -z "${SERNA_OUTPUT_DIR}" ]; then
    [ -d "${SERNA_OUTPUT_DIR}" ] || mkdir -p "${SERNA_OUTPUT_DIR}"
fi

# FIXME: java looks for dita-ot-settings.ent included from catalog-11.xml here
DITA_OT_SETTINGS=${SERNA_OUTPUT_DIR}/dita-ot-settings.ent
cp ${DITA_DIR}/../dita-ot-settings.ent ${DITA_OT_SETTINGS}

${ANT_HOME}/bin/ant --noconfig  -Dargs.input=${SERNA_XML_SRCFULLPATH} \
    -Doutput.dir="${SERNA_OUTPUT_DIR}" -Ddita.temp.dir="${SERNA_TEMPDIR}" \
    -Dargs.xhtml.toc="${SERNA_XML_SRCFILENAME}" -Dargs.logdir="${SERNA_TEMPDIR}" \
    -Dargs.xsl="${SERNA_XSL_STYLESHEET}" \
    -f "${DITA_DIR}/build.xml" ${ANT_TARGET}

test -f ${CATMGRPROPS} && rm -f ${CATMGRPROPS}
test -f ${DITA_OT_SETTINGS} && rm -f ${DITA_OT_SETTINGS}
