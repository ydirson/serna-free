#
#

TEMPLATE = serna
LIBRARIES = spgrovebuilder
CONFIG += qtexternal

INCLUDEPATH =	\
		. ; \
		$$QT_INC; \
		$(CLIB_SRC); \
		$(THIRD_DIR)/jade; \
                $(THIRD_DIR)/qt/include/QtCore

win32:DEFINES *= BUILD_SPGROVEBUILDER SPGROVEBUILDER_API

debug:DEFINES += GROVE_DEBUG
static:DEFINES *= SP_DEFINE_TEMPLATES
unix:DEFINES -= SP_DEFINE_TEMPLATES

LIBS = 	\
    $(CLIB_LIB)/common \
    $(CLIB_LIB)/grove \
    $(CLIB_LIB)/dav \
    $(THIRD_DIR)/lib/sp 

qtexternal:LIBS += $(THIRD_DIR)/lib/QtCore

SOURCES_PATTERN = impl/.*\.cxx$
HEADERS_PATTERN += .*\.h$ \
                   impl/.*\.h$

EXTRA_TEMPLATES = genlist 
