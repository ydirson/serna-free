#
#

TEMPLATE = serna
LIBRARIES = plaineditor
CONFIG += moc qtexternal

DEFINES += BUILD_PLAINEDITOR

INCLUDEPATH =   \
                .; \
                ..; \
                $(srcdir); \
                $(srcdir)/..; \
                $(top_builddir)/sfworks; \
                $(CLIB_SRC); \

LIBS =  \
        $(CLIB_LIB)/common \
        $(CLIB_LIB)/dav \
        $(CLIB_LIB)/grove \
        $(CLIB_LIB)/spgrovebuilder \
        $(CLIB_LIB)/ui \
        $(CLIB_LIB)/groveeditor \
        $(top_builddir)/serna/lib/utils \
        $(top_builddir)/serna/lib/docview \
        $(top_builddir)/serna/lib/docutils 

USE             = QtCore QtGui Qt3Support QtXml QtNetwork qscintilla

HEADERS_PATTERN =   \.h$ \
                    impl/.*\.h$ \
                    qt/.*\.h$ 

SOURCES_PATTERN =   \.cxx$ \
                    impl/.*\.cxx$ \
                    qt/.*\.cxx$ 

SOURCE_SUFFIXES = msg
UI_DIRS         = $$srcdir/qt
ALL_DEPS       += $(msg_h)

EXTRA_TEMPLATES = genlist
