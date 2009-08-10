
TEMPLATE  = serna
LIBRARIES = docutils

DEFINES   *= BUILD_DOCTAGS DOCTAGS_API

SERNA_DOC  = DITA

DOCDIR     = $(top_srcdir)/serna/dist/doc
DOCBUILDDIR= $(top_builddir)/serna/dist/doc

INCLUDEPATH = \
              $(CLIB_SRC); \
              $(srcdir)/..; \
              $(DOCBUILDDIR); \

USE          = QtCore QtGui QtAssistantClient
LIBS        += $(CLIB_LIB)/common

SOURCES     += $$srcdir/impl/get_tag.cxx

EXTRA_TEMPLATES = doc

SAPI_HTML_DIR       = $(DOCBUILDDIR)/sapi_py
SAPI_PY_HTML_DIR    = $(DOCBUILDDIR)/sapi

EXTRA_TEMPLATES = doc

XSLTPROC_CMD = $(THIRD_DIR)/bin/xsltproc
win32:XSLTPROC_CMD = $(THIRD_DIR)/bin/xsltproc.cmd
DOXYGEN = $(THIRD_DIR)/bin/doxygen

DITA2HTML       = $(THIRD_DIR)/bin/dita2html
DOCBOOK2HTML    = $(THIRD_DIR)/bin/docbook2html

DB2DITA_DIR         = $(top_srcdir)/serna/docutils/migration
DITA_SRC_DIR        = $(DOCDIR)-dita
DITA_XSL_DIR        = $$srcdir/xsl/dita
DITA_HTML_DIR       = $(DOCBUILDDIR)-dita
DITA_HTML_XSL       = $$DITA_XSL_DIR/dita2html.xsl
SERNA_HELP_DITAMAP  = $(DOCDIR)-dita/serna-help.ditamap

DITA_HTML_TEMPDIR   = temp

BOOSTBOOK_DIR       = $(top_srcdir)/tools/boostbook

XML_CATALOG_FILES   = \
                    $(top_srcdir)/serna/dist/plugins/docbook/dbk42/dtd/catalog.xml \
                    $(top_srcdir)/serna/dist/plugins/docbook/dbk45/dtd/catalog.xml \
                    $(THIRD_DIR)/dita-ot/dita-dir-catalog.xml
TAG_CHK_SCRIPT      = $(srcdir)/check_tags.py

NOTPARALLEL = 1

QHG = $(THIRD_DIR)/bin/qhelpgenerator
QCG = $(THIRD_DIR)/bin/qcollectiongenerator
