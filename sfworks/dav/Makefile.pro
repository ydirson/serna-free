#
#

TEMPLATE = serna
LIBRARIES = dav
CONFIG += moc

INCLUDEPATH =	$(CLIB_SRC)

win32:DEFINES += BUILD_DAV DAV_API

USE              = QtCore
LIBS             = $(CLIB_LIB)/common 

HEADERS_PATTERN = \.h$ impl/\.h$
SOURCES_PATTERN = impl/\.cxx$

EXTRA_TEMPLATES = genlist
