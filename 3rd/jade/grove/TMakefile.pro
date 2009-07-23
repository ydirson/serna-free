#
#

TEMPLATE = 3rd
TARGET = grove
options = package=jade,buildtype=tmake
EXTRA_TEMPLATES = genlist
win32:DEFINES	+= BUILD_LIBGROVE

LIB_DESTDIR             = $$PACKAGE_LIB_DESTDIR
BIN_DESTDIR             = $$PACKAGE_BIN_DESTDIR
