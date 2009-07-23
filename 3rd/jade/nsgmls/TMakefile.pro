#
#

TEMPLATE = 3rd
LIBS += ../lib/sp
win32:LIBS += ole32.lib gdi32.lib
INCLUDEPATH    += ../libsp
CONFIG += app
TARGET = nsgmls
EXTRA_TEMPLATES = genlist
DEFINES += SP_NAMESPACE=Sp DSSSL_NAMESPACE=Dsssl
BIN_DESTDIR	        = $(top_builddir)/../bin
