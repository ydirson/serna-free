
TEMPLATE = serna
LIBRARIES = ui
CONFIG += moc

INCLUDEPATH =	$(CLIB_SRC); \
                .; \

win32:DEFINES += BUILD_UI UI_API QT_QTPROPERTYBROWSER_IMPORT

LIBS = 	\
        $(CLIB_LIB)/common \
        $(CLIB_LIB)/dav \

USE = QtCore QtGui qtpropertybrowser sp

HEADERS_PATTERN =   .h$ \
                    impl/.*\.h$ \
                    impl/qt/.*\.h$

SOURCES_PATTERN =   impl/.*\.cxx$ \
                    impl/qt/.*\.cxx$

EXTRA_TEMPLATES = genlist
