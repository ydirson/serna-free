#
#

TEMPLATE = serna
LIBRARIES = proputils
CONFIG += qtexternal

DEFINES += TRACE_ON
win32:DEFINES += BUILD_PROPUTILS PROPUTILS_API

INCLUDEPATH = 	$(CLIB_SRC); \
                $(THIRD_DIR)/qt/include/QtCore

LIBS                = $(CLIB_LIB)/spgrovebuilder \
                      $(CLIB_LIB)/catmgr \
                      $(CLIB_LIB)/grove \
                      $(CLIB_LIB)/common 

qtexternal:LIBS += $(THIRD_DIR)/lib/QtCore

EXTRA_TEMPLATES = genlist
