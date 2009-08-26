#
#

TEMPLATE = serna
LIBRARIES = utils
CONFIG += moc

DEFINES *= BUILD_UTILS
NOTPARALLEL = 1

INCLUDEPATH =    \
                .; \
                ..; \
                $(srcdir); \
                $(srcdir)/impl; \
                $(top_builddir)/sfworks; 

win32:INCLUDEPATH *= $(srcdir)/impl/win32;
unix:INCLUDEPATH *= $(srcdir)/impl/unix;

INCLUDEPATH +=  \
                $(srcdir)/..; \
                $(CLIB_SRC); \
                $(top_srcdir)/sfworks; \

USE  = QtCore QtGui QtAssistantClient

LIBS += \
           $(CLIB_LIB)/grove \
           $(CLIB_LIB)/spgrovebuilder \
           $(CLIB_LIB)/catmgr \
           $(CLIB_LIB)/xpath \
           $(CLIB_LIB)/ui \
           $(CLIB_LIB)/sxslt \
           $(CLIB_LIB)/urimgr \
           $(CLIB_LIB)/dav \
           $(CLIB_LIB)/common \
           $(CLIB_LIB)/proputils \
           $(CLIB_LIB)/formatter \
           $(CLIB_LIB)/xs \
           $(top_builddir)/serna/lib/docutils

HEADERS_PATTERN = \
		  impl/.*\.h$ \
		  \.h$ 

SOURCES_PATTERN = \
                  \.cxx$ \
                  impl/.*\.cxx$ 

darwin:SOURCES_PATTERN *= impl/darwin/.*\.cxx$
linux:SOURCES_PATTERN *= impl/unix/.*\.cxx$
sunos:SOURCES_PATTERN *= impl/unix/.*\.cxx$
win32:SOURCES_PATTERN *= impl/win32/.*\.cxx$

darwin:USE *= QtNetwork sp

EXTRA_TEMPLATES = genlist appver
APPVER_FILE     = $(srcdir)/../app/APPVER
UI_DIRS        = $$srcdir/qt
