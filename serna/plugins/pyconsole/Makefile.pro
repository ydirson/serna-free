#
#
TEMPLATE  = serna

LIBRARIES = pyconsole
CONFIG   += qtexternal moc dynamic dll

INCLUDEPATH =   .; \
                $(top_builddir)/serna; \
                $(CLIB_SRC); \
                $(top_srcdir)/serna; \
                $(THIRD_DIR)/qt/include/QtCore; \
                $(THIRD_DIR)/qt/include/QtGui;

qtexternal:LIBS += $(THIRD_DIR)/lib/QtCore \
                   $(THIRD_DIR)/lib/QtGui \
                   $(THIRD_DIR)/lib/QtXml \
                   $(THIRD_DIR)/lib/QtNetwork

HEADERS_PATTERN = \.h$ 
                 
SOURCES_PATTERN = \.cxx$ 
USE_SAPI = 1

EXTRA_TEMPLATES = genlist ../plugins_common 
DESTDIR = $$top_builddir/serna/lib/pyplugin
UI_DIRS        = $$srcdir
