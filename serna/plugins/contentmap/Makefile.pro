#
#

TEMPLATE  = serna

LIBRARIES = contentmap
CONFIG += moc dll dynamic

win32:DEFINES += BUILD_CONTENTMAP BUILD_CSL
win32:DEFINES -= SERNA_DLL SFWORKS_DLL

INCLUDEPATH =    \
                .; \
                ..; \
                $(srcdir)/../..; \
                $(srcdir)/..; \
                $(srcdir); \
                $(srcdir)/qt; \
                $(srcdir)/csl; \
                $(srcdir)/impl; \
                $(top_builddir)/serna; \
                $(top_builddir)/sfworks; \
                $(CLIB_SRC); \ 

debug:LIBS +=	$(CLIB_LIB)/common \
                $(CLIB_LIB)/grove \
                $(CLIB_LIB)/ui \
                $(CLIB_LIB)/formatter \
                $(CLIB_LIB)/editableview \
                $(CLIB_LIB)/groveeditor \
                $(CLIB_LIB)/xpath \
                $(CLIB_LIB)/urimgr \
                $(CLIB_LIB)/catmgr \
                $(CLIB_LIB)/xs \
                $(CLIB_LIB)/sxslt \
                $(CLIB_LIB)/proputils \
                $(top_builddir)/serna/lib/docview \
                $(top_builddir)/serna/lib/utils \
                $(top_builddir)/serna/lib/structeditor \
                $(top_builddir)/serna/lib/docutils 

HEADERS_PATTERN = \.h$ \
                  csl/.*\.h$ \
                  qt/.*\.h$ 

SOURCES_PATTERN = .*\.cxx$ \
                  csl/.*\.cxx$ \
                  qt/.*\.cxx$ 

USE = QtGui QtCore Qt3Support QtXml QtNetwork

EXTRA_TEMPLATES = genlist ../plugins_common
DESTDIR = $$top_builddir/serna/lib/contentmap
UI_DIRS        = $$srcdir
