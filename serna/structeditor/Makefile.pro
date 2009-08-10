#
#

TEMPLATE = serna
LIBRARIES = structeditor
CONFIG += moc

dynamic:DEFINES += BUILD_STRUCTEDITOR STRUCTEDITOR_API
release:DEFINES += STRUCTEDITOR_API
win32:DEFINES += QT_INTERNAL_CANVAS

INCLUDEPATH =   \
                .; \
                ..; \
                $(srcdir); \
                $(srcdir)/..; \
                $(srcdir)/impl; \
                $(top_builddir)/sfworks; \
                $(CLIB_SRC);

LIBS =  \
	$(CLIB_LIB)/formatter \
        $(CLIB_LIB)/editableview \
        $(CLIB_LIB)/common \
        $(CLIB_LIB)/dav \
        $(CLIB_LIB)/grove \
        $(CLIB_LIB)/proputils \
        $(CLIB_LIB)/ui \
        $(CLIB_LIB)/spgrovebuilder \
        $(CLIB_LIB)/groveeditor \
        $(CLIB_LIB)/urimgr \
        $(CLIB_LIB)/xpath \
        $(CLIB_LIB)/sxslt \
        $(CLIB_LIB)/xs \
        $(CLIB_LIB)/proputils \
        $(top_builddir)/serna/lib/docview \
        $(top_builddir)/serna/lib/utils \
        $(top_builddir)/serna/lib/docutils

sceneareaset:LIBS  += $(CLIB_LIB)/sceneareaset
canvasareaset:LIBS += $(CLIB_LIB)/canvasareaset

USE              = QtCore QtGui Qt3Support QtXml QtNetwork

HEADERS_PATTERN =   \.h$ \
                    qt/.*\.h$ \
                    impl/.*\.h$

SOURCES_PATTERN =   \.cxx$ \
                    qt/.*\.cxx$  \
                    impl/.*\.cxx$

UI_DIRS         = $$srcdir/qt
ui_PATTERN      = $$UI_DIRS/.*\.ui$

EXTRA_TEMPLATES = genlist
