#
#

TEMPLATE = 3rd
TARGET = spgrove
EXTRA_TEMPLATES = genlist
win32:DEFINES = BUILD_LIBSPGROVE
INCLUDEPATH += ../grove
LIBS	= ../lib/sp ../lib/grove

LIB_DESTDIR             = $$PACKAGE_LIB_DESTDIR
BIN_DESTDIR             = $$PACKAGE_BIN_DESTDIR
