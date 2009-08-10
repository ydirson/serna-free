#
#

TEMPLATE = serna
LIBRARIES = docview
CONFIG += moc

DEFINES *= BUILD_DOCVIEW

INCLUDEPATH =    \
                .; \
                ..; \
                $(srcdir); \
                $(srcdir)/impl; \
                $(top_builddir); \
                $(top_builddir)/sfworks; \    
                $(srcdir)/..; \
                $(CLIB_SRC); \

LIBS +=	        $(CLIB_LIB)/common \
                $(CLIB_LIB)/ui \
                $(CLIB_LIB)/dav \
                $(CLIB_LIB)/grove \
                $(CLIB_LIB)/proputils \
                $(CLIB_LIB)/spgrovebuilder \
                $(CLIB_LIB)/xs \
                $(top_builddir)/serna/lib/utils \
                $(top_builddir)/serna/lib/docutils 

dynamic:LIBS +=	$(CLIB_LIB)/proputils

USE             = QtCore QtGui QtNetwork QtAssistantClient

HEADERS_PATTERN = \
                  .h$ \
                  qt/.*\.h$

SOURCES_PATTERN = \
                  \.cxx$ \
                  qt/.*\.cxx$ \
                  impl/.*\.cxx$ 

EXTRA_TEMPLATES = genlist 
APPVER_FILE     = $(srcdir)/../app/APPVER
UI_DIRS        = $$srcdir/qt
ui_PATTERN      = $$UI_DIRS/.*\.ui$
