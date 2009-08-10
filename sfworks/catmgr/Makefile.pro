
TEMPLATE = serna
LIBRARIES = catmgr
CONFIG -= oldwin32

INCLUDEPATH = \
              .; \
              $$srcdir; \
              $$srcdir/impl; \
              $(CLIB_SRC); \

LIBS        = \
              $(CLIB_LIB)/common \
              $(CLIB_LIB)/grove \
              $(CLIB_LIB)/spgrovebuilder \

USE            = QtCore

win32:DEFINES += BUILD_CATMGR CATMGR_API

SOURCES_PATTERN = impl/.*\.cxx$
HEADERS_PATTERN += .*\.h$ \
                   impl/.*\.h$

SOURCE_SUFFIXES  = msg
msg_PATTERN = impl/.*\.msg$
    
EXTRA_TEMPLATES = genlist 

ALL_DEPS += $(msg_h)
msg_TARGET_SUFFIXES += cpp
