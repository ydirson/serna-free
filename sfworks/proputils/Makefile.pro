#
#

TEMPLATE = serna
LIBRARIES = proputils

DEFINES += TRACE_ON
win32:DEFINES += BUILD_PROPUTILS PROPUTILS_API

INCLUDEPATH = $(CLIB_SRC);

LIBS                = $(CLIB_LIB)/spgrovebuilder \
                      $(CLIB_LIB)/catmgr \
                      $(CLIB_LIB)/grove \
                      $(CLIB_LIB)/common 


USE = QtCore

EXTRA_TEMPLATES = genlist
