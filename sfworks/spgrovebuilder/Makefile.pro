#
#

TEMPLATE = serna
LIBRARIES = spgrovebuilder

INCLUDEPATH =	\
		. ; \
		$(CLIB_SRC);

win32:DEFINES *= BUILD_SPGROVEBUILDER SPGROVEBUILDER_API

debug:DEFINES += GROVE_DEBUG
static:DEFINES *= SP_DEFINE_TEMPLATES
unix:DEFINES -= SP_DEFINE_TEMPLATES

LIBS = 	\
    $(CLIB_LIB)/common \
    $(CLIB_LIB)/grove \
    $(CLIB_LIB)/dav

USE = sp QtCore


#    $(THIRD_DIR)/lib/sp 

#qtexternal:LIBS += $(THIRD_DIR)/lib/QtCore

SOURCES_PATTERN = impl/.*\.cxx$
HEADERS_PATTERN += .*\.h$ \
                   impl/.*\.h$

EXTRA_TEMPLATES = genlist 
