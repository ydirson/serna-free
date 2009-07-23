#
#

TEMPLATE = serna
LIBRARIES = catmgr
CONFIG -= oldwin32

INCLUDEPATH = \
              .; \
              $$srcdir; \
              $$srcdir/impl; \
              $(CLIB_SRC); \
              $(THIRD_DIR)/qt/include/QtCore; \
              $$QT_INC

LIBS        = \
              $(CLIB_LIB)/common \
              $(CLIB_LIB)/grove \
              $(CLIB_LIB)/spgrovebuilder \
              $(THIRD_DIR)/lib/QtCore
              
win32:DEFINES += BUILD_CATMGR CATMGR_API

SOURCES_PATTERN = impl/.*\.cxx$
HEADERS_PATTERN += .*\.h$ \
                   impl/.*\.h$

SOURCE_SUFFIXES  = msg
msg_PATTERN = impl/.*\.msg$
    
EXTRA_TEMPLATES = genlist 

ALL_DEPS += $(msg_h)
msg_TARGET_SUFFIXES += cpp
