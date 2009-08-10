#
#

TEMPLATE = serna
LIBRARIES = grove

INCLUDEPATH =	.;\
		$$QT_INC; \
		$(CLIB_SRC)

#		$(THIRD_DIR)/jade; \
#                $(THIRD_DIR)/qt/include/QtCore

debug:DEFINES *= GROVE_DEBUG GROVE_API

win32:DEFINES += BUILD_GROVE
#win32:WIN32_RESOURCE	= $(CLIB_SRC)/grove/GroveVersion.rc

USE = QtCore sp

LIBS = 	\
    $(CLIB_LIB)/common \
    $(CLIB_LIB)/dav

SOURCE_SUFFIXES  = msg
msg_PATTERN = impl/.*\.msg$

HEADERS_PATTERN = \.h$ impl/\.h$
SOURCES_PATTERN = impl/\.cxx$
EXTRA_TEMPLATES = genlist

ALL_DEPS += $(msg_h)
msg_TARGET_SUFFIXES += cpp

