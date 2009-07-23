
DB2DITA_DIR     = $(top_srcdir)/serna/docutils/migration
DB2DITA_XSL     = $(DB2DITA_DIR)/docbook-to-dita.xsl

DITA_GEN_DIR    = $(top_builddir)/serna/dist/doc-db2dita

SERNA_HELP_SRC  = $$DOCDIR/serna-help-structure.xml

SERNA_HELP_MAP  = $(DITA_GEN_DIR)/serna-help.ditamap

CHECK_ELEMS     = $(DB2DITA_DIR)/check-topic-elems.xsl
CHECK_TEXT_XSL  = $(DB2DITA_DIR)/check-text.xsl
CHECK_TEXT_PY   = $(DB2DITA_DIR)/check-text.py

DB2DITA_PARAMS  = $(DB2DITA_DIR)/docbook-to-dita-param.xsl

