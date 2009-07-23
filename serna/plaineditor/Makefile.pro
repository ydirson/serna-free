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
                $(THIRD_DIR)/qt/include/QtCore; \
                $(THIRD_DIR)/qt/include/QtGui; \
                $(THIRD_DIR)/qt/include/Qt3Support

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

qtexternal:LIBS += $(THIRD_DIR)/lib/qscintilla2
qtexternal:LIBS += $(THIRD_DIR)/lib/QtCore \
                   $(THIRD_DIR)/lib/QtGui \
                   $(THIRD_DIR)/lib/Qt3Support \
                   $(THIRD_DIR)/lib/QtXml \
                   $(THIRD_DIR)/lib/QtNetwork

HEADERS_PATTERN =   \.h$ \
                    impl/.*\.h$ \
                    qt/.*\.h$ 

SOURCES_PATTERN =   \.cxx$ \
                    impl/.*\.cxx$ \
                    qt/.*\.cxx$ 

SOURCE_SUFFIXES = msg
UI_DIRS		= $$srcdir/qt
ALL_DEPS += $(msg_h)

EXTRA_TEMPLATES = genlist

