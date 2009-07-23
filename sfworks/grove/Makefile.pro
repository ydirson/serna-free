#
#

TEMPLATE = serna
LIBRARIES = grove
CONFIG += qtexternal

INCLUDEPATH =	.;\
		$$QT_INC; \
		$(CLIB_SRC); \
		$(THIRD_DIR)/jade; \
                $(THIRD_DIR)/qt/include/QtCore

debug:DEFINES *= GROVE_DEBUG GROVE_API

win32:DEFINES += BUILD_GROVE
#win32:WIN32_RESOURCE	= $(CLIB_SRC)/grove/GroveVersion.rc

LIBS = 	\
    $(CLIB_LIB)/common \
    $(CLIB_LIB)/dav \
    $(THIRD_DIR)/lib/sp 
    
qtexternal:LIBS += $(THIRD_DIR)/lib/QtCore

SOURCE_SUFFIXES  = msg
msg_PATTERN = impl/.*\.msg$

HEADERS_PATTERN = \.h$ impl/\.h$
SOURCES_PATTERN = impl/\.cxx$
EXTRA_TEMPLATES = genlist

ALL_DEPS += $(msg_h)
msg_TARGET_SUFFIXES += cpp

