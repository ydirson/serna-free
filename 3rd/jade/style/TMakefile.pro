#
#

TEMPLATE = 3rd
TARGET = style
INCLUDEPATH += ../grove ../spgrove
EXTRA_TEMPLATES = genlist
win32:DEFINES += SP_DEFINE_TEMPLATES
LIBS += ../lib/sp ../lib/spgrove ../lib/grove

LIB_DESTDIR             = $$PACKAGE_LIB_DESTDIR
BIN_DESTDIR             = $$PACKAGE_BIN_DESTDIR
