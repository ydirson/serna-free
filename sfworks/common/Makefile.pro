#
#
TEMPLATE = serna
LIBRARIES = common
CONFIG += qtexternal

INCLUDEPATH += \
               $$QT_INC; \
               $(CLIB_SRC); \
               $(top_builddir)/sfworks; \
               $$srcdir/zipfile; \
               $$srcdir/impl; \
               $(THIRD_DIR)/qt/include/QtCore;

unix:INCLUDEPATH += $$srcdir/impl/unix;

INCLUDEPATH += $$srcdir/impl/$$PLATFORM;

win32:DEFINES *= BUILD_COMMON

win32:LIBS += ole32.lib \
              ws2_32.lib \
              kernel32.lib 

qtexternal:LIBS += $(THIRD_DIR)/lib/QtCore

EXTRA_TEMPLATES = genlist
DESTDIR = ../../sfworks/lib

CPP_WILDCARD = .*\.c((pp)|(xx))$

SOURCES_PATTERN        = impl/$$CPP_WILDCARD
unix:SOURCES_PATTERN  += impl/unix/$$CPP_WILDCARD
win32:SOURCES_PATTERN += impl/win32/$$CPP_WILDCARD
SOURCES_PATTERN       += zipfile/(ioapi|mztools|unzip|zip)\.c$
win32:SOURCES_PATTERN += zipfile/iowin32.c

SOURCES_EXCLUDE_PATTERN        = impl/FreeStorage\.cxx$
                                                                              
HEADERS_PATTERN        = .*\.h$

win32:PROFILE_SRC = $$srcdir/impl/pure_api.c
profile:SOURCES *= $$PROFILE_SRC
ALL_DEPS                += $(msg_h)
