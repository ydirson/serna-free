#

TEMPLATE  = serna

LIBRARIES = xsltparam
CONFIG += moc dll dynamic

win32:DEFINES += BUILD_XSLTPARAM
win32:DEFINES -= SERNA_DLL SFWORKS_DLL

INCLUDEPATH =    \
                .; \
                $(srcdir)/../..; \
                $(srcdir); \
                $(srcdir)/impl; \
                $(top_builddir)/sfworks; \ 
                $(top_builddir)/serna; \   
                $(srcdir)/..; \
                $(CLIB_SRC); 

debug:LIBS +=	$(CLIB_LIB)/common \
                $(CLIB_LIB)/ui \
                $(top_builddir)/sfworks/lib/xpath \
                $(top_builddir)/sfworks/lib/sxslt \
                $(top_builddir)/sfworks/lib/proputils \
                $(top_builddir)/serna/lib/docview \
                $(top_builddir)/serna/lib/utils \
                $(top_builddir)/serna/lib/structeditor \
                $(top_builddir)/serna/lib/docutils

USE = QtGui QtCore

HEADERS_PATTERN = \.h$ qt/.*\.h$ 
SOURCES_PATTERN = .*\.cxx$ qt/.*\.cxx$ 
UI_DIRS		= $$srcdir/qt
ui_PATTERN      = $$UI_DIRS/.*\.ui$

EXTRA_TEMPLATES = genlist ../plugins_common
DESTDIR = $$top_builddir/serna/lib/xsltparam
