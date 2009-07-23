#
#

TEMPLATE = 3rd
LIBS += ../lib/style ../lib/spgrove ../lib/sp ../lib/grove
win32:LIBS += ole32.lib gdi32.lib
INCLUDEPATH    += ../style ../grove ../spgrove
CONFIG += app
TARGET = jade
EXTRA_TEMPLATES = genlist
DEFINES += SP_NAMESPACE=Sp DSSSL_NAMESPACE=Dsssl
BIN_DESTDIR	        = $(top_builddir)/../bin
