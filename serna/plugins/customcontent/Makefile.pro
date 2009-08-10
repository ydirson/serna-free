#
#

TEMPLATE  = serna

LIBRARIES = customcontent
CONFIG += moc dll dynamic

win32:DEFINES += BUILD_CUSTOMCONTENT
win32:DEFINES -= SERNA_DLL SFWORKS_DLL

INCLUDEPATH =    \
                .; \
                ..; \
                $(srcdir)/../..; \
                $(srcdir); \
                $(srcdir)/impl; \
                $(top_builddir)/sfworks; \    
                $(top_builddir)/serna; \
                $(srcdir)/..; \
                $(CLIB_SRC);

debug:LIBS +=	$(CLIB_LIB)/common \
                $(CLIB_LIB)/grove \
                $(CLIB_LIB)/ui \
                $(CLIB_LIB)/formatter \
                $(CLIB_LIB)/groveeditor \
                $(CLIB_LIB)/editableview \
                $(CLIB_LIB)/xpath \
                $(CLIB_LIB)/sxslt \
                $(CLIB_LIB)/proputils \
                $(top_builddir)/serna/lib/docview \
                $(top_builddir)/serna/lib/utils \
                $(top_builddir)/serna/lib/structeditor \
                $(top_builddir)/serna/lib/docutils 

HEADERS_PATTERN = \.h$ 

SOURCES_PATTERN = \
                  \.cxx$ 

USE = QtGui QtCore

EXTRA_TEMPLATES = genlist ../plugins_common
#APPVER_FILE     = $(srcdir)/../app/APPVER
DESTDIR = $$top_builddir/serna/lib/customcontent
UI_DIRS        = $$srcdir
