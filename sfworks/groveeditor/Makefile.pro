#
#

TEMPLATE = serna
LIBRARIES = groveeditor
CONFIG += qtexternal

INCLUDEPATH =	\
		$(THIRD_DIR)/qt/include/QtCore; \
                $(top_builddir)/sfworks; \
		$(CLIB_SRC);

DEFINES *= TRACE_ON
win32:DEFINES *= BUILD_GROVEEDITOR GROVEEDITOR_API

LIBS = 	\
	$(CLIB_LIB)/common \
        $(CLIB_LIB)/grove \
        $(CLIB_LIB)/dav
        
qtexternal:LIBS += $(THIRD_DIR)/lib/QtCore

EXTRA_TEMPLATES = genlist
ALL_DEPS                += $(msg_h)
SOURCES_PATTERN = impl/.*\.cxx$

SOURCE_SUFFIXES  = msg

msg_PATTERN = impl/.*\.msg$
msg_TARGET_SUFFIXES += cpp
