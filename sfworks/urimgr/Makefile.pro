#
#

TEMPLATE = serna
LIBRARIES = urimgr

INCLUDEPATH =   \
                $(CLIB_SRC); 

win32:DEFINES += BUILD_URI URI_API

USE = QtCore

LIBS = 	\
    $(CLIB_LIB)/common \
    $(CLIB_LIB)/grove \
    $(CLIB_LIB)/dav \
    $(CLIB_LIB)/catmgr \
    $(CLIB_LIB)/spgrovebuilder 

HEADERS_PATTERN = \.h$ impl/.*\.h$
SOURCES_PATTERN = impl/.*\.cxx$

EXTRA_TEMPLATES = genlist
