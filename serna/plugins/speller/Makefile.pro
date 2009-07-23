#
#
TEMPLATE  = serna

LIBRARIES = speller
CONFIG += moc qtexternal dll dynamic qt4

win32:DEFINES += BUILD_SPELLER
win32:DEFINES -= SERNA_DLL SFWORKS_DLL

INCLUDEPATH =    \
                .; \
                $(srcdir)/../..; \
                $(srcdir); \
                $(srcdir)/qt; \
                $(top_builddir)/sfworks; \ 
                $(top_builddir)/serna; \
                $(srcdir)/..; \
                $(CLIB_SRC); \
                $(THIRD_DIR)/qt/include/QtCore; \
                $(THIRD_DIR)/qt/include/Qt3Support; \
                $(THIRD_DIR)/qt/include/QtGui;

debug:LIBS +=   $(CLIB_LIB)/common \
                $(CLIB_LIB)/ui \
                $(top_builddir)/sfworks/lib/grove \
                $(top_builddir)/sfworks/lib/groveeditor \
                $(top_builddir)/sfworks/lib/formatter \
                $(top_builddir)/sfworks/lib/proputils \
                $(top_builddir)/sfworks/lib/xs \
                $(top_builddir)/sfworks/lib/sxslt \
                $(top_builddir)/sfworks/lib/editableview \
                $(top_builddir)/serna/lib/structeditor \
                $(top_builddir)/serna/lib/docview \
                $(top_builddir)/serna/lib/utils \
                $(top_builddir)/serna/lib/docutils

qtexternal:LIBS += $(THIRD_DIR)/lib/QtCore \
                   $(THIRD_DIR)/lib/QtGui \
                   $(THIRD_DIR)/lib/QtXml \
                   $(THIRD_DIR)/lib/QtNetwork

HEADERS_PATTERN = \.h$ \
                  aspell/.*\.h$ \
                  aspell/.*\.hpp$ \
                  qt/.*\.h$ 

SOURCES_PATTERN = .*\.cxx$ \
                  aspell/.*\.cxx$ \
                  qt/.*\.cxx$ 

UI_DIRS		= $$srcdir/qt
ui_PATTERN      = $$UI_DIRS/.*\.ui$

EXTRA_TEMPLATES += genlist ../plugins_common

DESTDIR = $$top_builddir/serna/lib/speller
