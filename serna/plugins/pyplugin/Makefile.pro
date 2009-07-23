#
#

TEMPLATE  = serna

LIBRARIES = pyplugin
CONFIG += qtexternal dll dynamic

DEFINES       += TRACE_ON
win32:DEFINES += BUILD_PYPLUGIN

INCLUDEPATH = 	$$QT_INC; \
                $(top_builddir)/serna; \
                $(srcdir)/../..;\
                $(CLIB_SRC);\
                $(THIRD_DIR)/python; \
                $(THIRD_DIR)/python/include;

win32:INCLUDEPATH += $(THIRD_DIR)/python/2.6.2/Python-2.6.2/PC;
debug:W32_LIBS += $(THIRD_DIR)/lib/python


qtexternal:LIBS += $(THIRD_DIR)/lib/$$QTLIB
linux:LIBS      += dl
win32:LIBS      += $$W32_LIBS

USE_SAPI = 1
HEADERS_PATTERN = \.h$ \
                  impl/.*\.h$ 

SOURCES_PATTERN = .*\.cxx$ 

EXTRA_TEMPLATES += genlist ../plugins_common

DESTDIR = $$top_builddir/serna/lib/pyplugin
